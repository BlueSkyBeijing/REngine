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
#include "MathUtility.h"

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

    createPassConstantBuffer();
    mRHI->FlushCommandQueue();

}

void FRenderer::UnInit()
{
    mRHI->FlushCommandQueue();

    mPassConstantBuffer->UnInit();
    delete mPassConstantBuffer;
    mPassConstantBuffer = nullptr;
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
    updatePassConstantBuffer();
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
        mRHI->SetConstantBuffer(renderProxy->ConstantBuffer);
        mRHI->SetConstantBuffer(mPassConstantBuffer);

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
    _createPassConstant(constant);

    mPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FPassConstant), (uint8*)&constant, 3);
}

void FRenderer::updatePassConstantBuffer()
{
    //create pass constant buffer
    FPassConstant constant;
    _createPassConstant(constant);

    mRHI->UpdateConstantBuffer(mPassConstantBuffer, sizeof(FPassConstant), (uint8*)&constant);
}

void FRenderer::_createPassConstant(FPassConstant& constant)
{
    FMatrix4x4 viewMatrix;
    ConstructMatrixLookRight(viewMatrix, mView->Position, mView->Look, mView->Right);

    FMatrix4x4 projectionMatrix;
    const float fovY = mView->FOV;
    const float aspect = mView->AspectRatio;
    const float nearPlane = 1.0f;
    const float farPlane = 100000.0f;
    ConstructMatrixPerspectiveFovLH(projectionMatrix, fovY, aspect, nearPlane, farPlane);

    constant.View = viewMatrix;
    constant.Proj = projectionMatrix;
    constant.ViewProj = viewMatrix * projectionMatrix;

    const FVector3 cameraDirection = mView->Look;
    const FVector3 directonalLightDirection(-1.0f, -1.0f, -1.0f);
    const FVector3 directonalLightColor(1.0f, 1.0f, 1.0f);

    constant.CameraPos = FVector3(mView->Position.x(), mView->Position.y(), mView->Position.z());
    constant.CameraDir = FVector3(cameraDirection.x(), cameraDirection.y(), cameraDirection.z());
    //should use negative value of camera direction in shader
    constant.DirectionalLightDir = -FVector3(directonalLightDirection.x(), directonalLightDirection.y(), directonalLightDirection.z());
    constant.DirectionalLightColor = FVector3(directonalLightColor.x(), directonalLightColor.y(), directonalLightColor.z());

}
