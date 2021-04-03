#include "FRenderer.h"
#include "FRHIRenderTarget.h"
#include "FRenderProxy.h"
#include "FScene.h"
#include "FRHI.h"
#include "FPipelineStateManager.h"
#include "FShaderBindingsManager.h"
#include "FRHIShader.h"
#include "Utility.h"
#include "FRHIPipelineState.h"
#include "FRHIShaderBindings.h"
#include "TSingleton.h"
#include "FMaterial.h"
#include "FRHIBuffer.h"
#include "FView.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include <Eigen/Dense>

#include <vector>
#include <string>

FRenderer::FRenderer(FRHIRenderWindow* renderWindow, FScene* scene, FView* view) :
    mRHI(TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI()),
    mRenderWindow(renderWindow),
    mScene(scene),
    mView(view)
{
}

FRenderer::~FRenderer()
{
}

void FRenderer::Init()
{
    mRHI->FlushCommandQueue();
}

void FRenderer::UnInit()
{
    mRHI->FlushCommandQueue();

    mPassConstantBuffer->UnInit();
    delete mPassConstantBuffer;
    mPassConstantBuffer = nullptr;
}

void FRenderer::CreateRenderResources()
{
    createPassConstantBuffer();

    std::vector<FRenderProxy*>& renderProxys = mScene->GetRenderProxys();
    for (auto it = renderProxys.begin(); it != renderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;
        renderProxy->CreateRenderResource();
    }

    mRHI->FlushCommandQueue();
}

void FRenderer::preRender()
{
    mRHI->BeginCommmandList();
    mRHI->SetRenderTarget(mRenderWindow);
    mRHI->BeginDraw();
}

void FRenderer::RenderOneFrame()
{
    preRender();

    clear();

    initView();

    drawRenderables();

    postProcess();

    postRender();
}

void FRenderer::clear()
{
    const DirectX::XMFLOAT4 clearColor = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mRHI->Clear(clearColor);
}

void FRenderer::initView()
{
    setViewPort();
    computeVisibility();
}

void FRenderer::computeVisibility()
{
}

void FRenderer::setRenderTarget(FRHIRenderTarget* renderTarget)
{
    mRHI->SetRenderTarget(renderTarget);
}

void FRenderer::setViewPort()
{
    //viewport
    FViewPort viewPort = { 0.0f, 0.0f, static_cast<float>(mRenderWindow->Width), static_cast<float>(mRenderWindow->Height), 0.0f, 1.0f };
    //scissor rectangle
    FRect scissorRect = { 0, 0, static_cast<LONG>(mRenderWindow->Width), static_cast<LONG>(mRenderWindow->Height) };

    mRHI->SetViewPort(viewPort);
    mRHI->SetSetScissor(scissorRect);
}

void FRenderer::drawRenderables()
{
    const std::vector<FRenderProxy*>& renderProxys = mScene->GetRenderProxys();
    for (auto it = renderProxys.begin(); it != renderProxys.end(); it++)
    {
        FRenderProxy * renderProxy = *it;

        mRHI->BeginEvent(renderProxy->DebugName);

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetOrCreatePipleLineState(renderProxy);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveTopology(EPrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mRHI->SetVertexBuffer(renderProxy->VertexBuffer);
        mRHI->SetIndexBuffer(renderProxy->IndexBuffer);
        mRHI->DrawIndexedInstanced(renderProxy->IndexCountPerInstance, renderProxy->InstanceCount, renderProxy->StartIndexLocation, renderProxy->BaseVertexLocation, renderProxy->StartInstanceLocation);

        mRHI->EndEvent();
    }
}

void FRenderer::postProcess()
{
}

void FRenderer::postRender()
{
    mRHI->EndDraw();
    mRHI->EndCommmandList();

    mRHI->ExecuteCommandList();
    mRHI->FlushCommandQueue();
}

void FRenderer::createPassConstantBuffer()
{
    ////create pass constant buffer
    //FPassConstant constant;
    //FMatrix4x4 mViewMatrix;
    //FMatrix4x4 mProjMatrix;
    //
    ////build view matrix.
    //DirectX::XMVECTOR pos = DirectX::XMVectorSet(mView->Position.x, mView->Position.y, mView->Position.z, 1.0f);
    //DirectX::XMVECTOR target = DirectX::XMVectorSet(mView->Target.x, mView->Target.y, mView->Target.z, 1.0f);
    //DirectX::XMVECTOR up = DirectX::XMVectorSet(mView->Up.x, mView->Up.y, mView->Up.z, 1.0f);

    //DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
    //DirectX::XMStoreFloat4x4(&mViewMatrix, view);

    //const float AspectRatio = 1.7f;
    //DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, AspectRatio, 1.0f, 100000.0f);
    //DirectX::XMStoreFloat4x4(&mProjMatrix, P);

    //DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProjMatrix);

    //DirectX::XMStoreFloat4x4(&constant.View, DirectX::XMMatrixTranspose(view));
    //DirectX::XMStoreFloat4x4(&constant.ViewProj, DirectX::XMMatrixTranspose(view * proj));

    //mPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(constant), (uint8*)&constant, 3);

    //zaxis = normal(cameraTarget - cameraPosition)
    //    xaxis = normal(cross(cameraUpVector, zaxis))
    //    yaxis = cross(zaxis, xaxis)

    //    xaxis.x           yaxis.x           zaxis.x          0
    //    xaxis.y           yaxis.y           zaxis.y          0
    //    xaxis.z           yaxis.z           zaxis.z          0
    //    - dot(xaxis, cameraPosition) - dot(yaxis, cameraPosition) - dot(zaxis, cameraPosition)  1

    Eigen::Matrix4f ViewMatrix, ProjectionMatrix;
    Eigen::Vector3f position(mView->Position.x, mView->Position.y, mView->Position.z);
    Eigen::Vector3f target(mView->Target.x, mView->Target.y, mView->Target.z);
    Eigen::Vector3f up(mView->Up.x, mView->Up.y, mView->Up.z);

    Eigen::Vector3f zaxis = (target - position).normalized();
    Eigen::Vector3f xaxis = up.cross(zaxis).normalized();
    Eigen::Vector3f yaxis = zaxis.cross(xaxis);

    ViewMatrix.col(0) = Eigen::Vector4f(xaxis.x(), xaxis.y(), xaxis.z(), -xaxis.dot(position));
    ViewMatrix.col(1) = Eigen::Vector4f(yaxis.x(), yaxis.y(), yaxis.z(), -yaxis.dot(position));
    ViewMatrix.col(2) = Eigen::Vector4f(zaxis.x(), zaxis.y(), zaxis.z(), -zaxis.dot(position));
    ViewMatrix.col(3) = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

    //w       0       0                                             0
    //    0       h       0                                             0
    //    0       0       zfarPlane / (zfarPlane - znearPlane)              1
    //    0       0 - znearPlane * zfarPlane / (zfarPlane - znearPlane)  0

    float fovY = 3.1415f * 0.25f;
    float aspect = 1.7f;
    float nearPlane = 1.0f;
    float farPlane = 100000.0f;
    float theta = fovY * 0.5f;
    float range = farPlane - nearPlane;
    float invtan = 1. / tan(theta);

    ProjectionMatrix.setConstant(0.0f);
    ProjectionMatrix(0, 0) = invtan / aspect;
    ProjectionMatrix(1, 1) = invtan;
    ProjectionMatrix(2, 2) = farPlane /  range;
    ProjectionMatrix(2, 3) = 1;
    ProjectionMatrix(3, 2) = -nearPlane * farPlane / range;
    ProjectionMatrix(3, 3) = 0;

    struct ConstantMy
    {
        Eigen::Matrix4f View;
        Eigen::Matrix4f Proj;
        Eigen::Matrix4f ViewProj;
    };

    ConstantMy m;

    m.View = ViewMatrix;
    m.Proj = ProjectionMatrix;
    m.ViewProj = ViewMatrix * ProjectionMatrix;

    mPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(ConstantMy), (uint8*)&m, 3);
}
