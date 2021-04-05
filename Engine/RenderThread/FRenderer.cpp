#include "PrecompiledHeader.h"
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
    const FVector4 clearColor(0.5f, 0.5f, 0.5f, 1.0f);
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
    //create pass constant buffer
    FPassConstant constant;
    
    //build view matrix.
    //from:https://docs.microsoft.com/en-us/previous-versions/windows/desktop/bb281710(v=vs.85)
    //zaxis = normal(cameraTarget - cameraPosition)
    //    xaxis = normal(cross(cameraUpVector, zaxis))
    //    yaxis = cross(zaxis, xaxis)

    //    xaxis.x           yaxis.x           zaxis.x          0
    //    xaxis.y           yaxis.y           zaxis.y          0
    //    xaxis.z           yaxis.z           zaxis.z          0
    //    - dot(xaxis, cameraPosition) - dot(yaxis, cameraPosition) - dot(zaxis, cameraPosition)  1

    Eigen::Matrix4f viewMatrix, projectionMatrix;
    Eigen::Vector3f position(mView->Position.x(), mView->Position.y(), mView->Position.z());
    Eigen::Vector3f target(mView->Target.x(), mView->Target.y(), mView->Target.z());
    Eigen::Vector3f up(mView->Up.x(), mView->Up.y(), mView->Up.z());

    Eigen::Vector3f zaxis = (target - position).normalized();
    Eigen::Vector3f xaxis = up.cross(zaxis).normalized();
    Eigen::Vector3f yaxis = zaxis.cross(xaxis);

    viewMatrix.col(0) = Eigen::Vector4f(xaxis.x(), xaxis.y(), xaxis.z(), -xaxis.dot(position));
    viewMatrix.col(1) = Eigen::Vector4f(yaxis.x(), yaxis.y(), yaxis.z(), -yaxis.dot(position));
    viewMatrix.col(2) = Eigen::Vector4f(zaxis.x(), zaxis.y(), zaxis.z(), -zaxis.dot(position));
    viewMatrix.col(3) = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

    //from:https://docs.microsoft.com/en-us/previous-versions/windows/desktop/bb281727(v=vs.85)
    //    w       0       0                                             0
    //    0       h       0                                             0
    //    0       0       zfarPlane / (zfarPlane - znearPlane)          1
    //    0       0 - znearPlane * zfarPlane / (zfarPlane - znearPlane)  0
    float fovY = 3.1415f * 0.25f;
    float aspect = 1.7f;
    float nearPlane = 1.0f;
    float farPlane = 100000.0f;
    float theta = fovY * 0.5f;
    float range = farPlane - nearPlane;
    float invtan = 1.0f / tan(theta);

    projectionMatrix.setConstant(0.0f);
    projectionMatrix(0, 0) = invtan / aspect;
    projectionMatrix(1, 1) = invtan;
    projectionMatrix(2, 2) = farPlane /  range;
    projectionMatrix(2, 3) = 1;
    projectionMatrix(3, 2) = -nearPlane * farPlane / range;
    projectionMatrix(3, 3) = 0;

    constant.View = viewMatrix;
    constant.Proj = projectionMatrix;
    constant.ViewProj = viewMatrix * projectionMatrix;

    mPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(constant), (uint8*)&constant, 3);
}
