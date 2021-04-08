#include "PrecompiledHeader.h"

#include "FRenderer.h"
#include "FRHIRenderTarget.h"
#include "FRenderProxy.h"
#include "FScene.h"
#include "FRHI.h"
#include "FPipelineStateManager.h"
#include "FShaderBindingsManager.h"
#include "FRHIShader.h"
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

    const FRHITransitionInfo info(mRenderWindow, ACCESS_PRESENT, ACCESS_RENDER_TARGET);
    mRHI->Transition(info);
}

void FRenderer::Render()
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
        FRenderProxy* renderProxy = *it;

        mRHI->BeginEvent(renderProxy->DebugName);

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(renderProxy);

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
    const FRHITransitionInfo info(mRenderWindow, ACCESS_RENDER_TARGET, ACCESS_PRESENT);
    mRHI->Transition(info);

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

    FMatrix4x4 viewMatrix, projectionMatrix;
    const FVector3 cameraPos(mView->Position.x(), mView->Position.y(), mView->Position.z());
    const FVector3 cameraTarget(mView->Target.x(), mView->Target.y(), mView->Target.z());
    const FVector3 cameraUp(mView->Up.x(), mView->Up.y(), mView->Up.z());

    const FVector3 zaxis = (cameraTarget - cameraPos).normalized();
    const FVector3 xaxis = cameraUp.cross(zaxis).normalized();
    const FVector3 yaxis = zaxis.cross(xaxis);

    viewMatrix.col(0) = Eigen::Vector4f(xaxis.x(), xaxis.y(), xaxis.z(), -xaxis.dot(cameraPos));
    viewMatrix.col(1) = Eigen::Vector4f(yaxis.x(), yaxis.y(), yaxis.z(), -yaxis.dot(cameraPos));
    viewMatrix.col(2) = Eigen::Vector4f(zaxis.x(), zaxis.y(), zaxis.z(), -zaxis.dot(cameraPos));
    viewMatrix.col(3) = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

    //from:https://docs.microsoft.com/en-us/previous-versions/windows/desktop/bb281727(v=vs.85)
    //    w       0       0                                             0
    //    0       h       0                                             0
    //    0       0       zfarPlane / (zfarPlane - znearPlane)          1
    //    0       0 - znearPlane * zfarPlane / (zfarPlane - znearPlane)  0
    const float fovY = 3.1415f * 0.25f;
    const float aspect = 1.7f;
    const float nearPlane = 1.0f;
    const float farPlane = 100000.0f;
    const float theta = fovY * 0.5f;
    const float range = farPlane - nearPlane;
    const float invtan = 1.0f / tan(theta);

    projectionMatrix.setConstant(0.0f);
    projectionMatrix(0, 0) = invtan / aspect;
    projectionMatrix(1, 1) = invtan;
    projectionMatrix(2, 2) = farPlane / range;
    projectionMatrix(2, 3) = 1;
    projectionMatrix(3, 2) = -nearPlane * farPlane / range;
    projectionMatrix(3, 3) = 0;

    constant.View = viewMatrix;
    constant.Proj = projectionMatrix;
    constant.ViewProj = viewMatrix * projectionMatrix;

    const FVector3 cameraDirection = (cameraTarget - cameraPos).normalized();
    const FVector3 directonalLightDirection(-1.0f, -1.0f, -1.0f);
    const FVector3 directonalLightColor(1.0f, 1.0f, 1.0f);

    constant.CameraPos = FVector3(cameraPos.x(), cameraPos.y(), cameraPos.z());
    constant.CameraDir = FVector3(cameraDirection.x(), cameraDirection.y(), cameraDirection.z());
    //should use negative value of camera direction in shader
    constant.DirectionalLightDir = -FVector3(directonalLightDirection.x(), directonalLightDirection.y(), directonalLightDirection.z());
    constant.DirectionalLightColor = FVector3(directonalLightColor.x(), directonalLightColor.y(), directonalLightColor.z());

    mPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FPassConstant), (uint8*)&constant, 3);
}
