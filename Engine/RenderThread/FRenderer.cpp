﻿#include "PrecompiledHeader.h"

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
#include "FLight.h"
#include "FPostProcessing.h"

FRenderer::FRenderer(FRHIRenderWindow* renderWindow, FScene* scene, FView* view) :
    mRHI(TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI()),
    mRenderTarget(renderWindow),
    mScene(scene),
    mView(view),
    mShadowMap(nullptr),
    mSceneColor(nullptr),
    mSceneColorPassConstantBuffer(nullptr),
    mShadowPassConstantBuffer(nullptr)
{
}

FRenderer::~FRenderer()
{
}

void FRenderer::Init()
{
    mRHI->FlushCommandQueue();

    initShadowPass();
    initSceneColorPass();
    initPostProcess();
}

void FRenderer::UnInit()
{
    mRHI->FlushCommandQueue();

    unInitShadowPass();
    unInitSceneColorPass();
    unInitPostProcess();
}

void FRenderer::preRender()
{
    mRHI->BeginCommmandList();
}

void FRenderer::postRender()
{
    mRHI->EndCommmandList();

    mRHI->ExecuteCommandList();
    mRHI->FlushCommandQueue();
}

void FRenderer::Render()
{
    preRender();

    updateShadowPass();
    updateSceneColorPass();
    updatePostProcess();

    postRender();
}

void FRenderer::initShadowPass()
{
    //create shadow depth render target
    mShadowMap = mRHI->CreateRenderTarget(TSingleton<FConfigManager>::GetInstance().ShadowMapWidth,
        TSingleton<FConfigManager>::GetInstance().ShadowMapHeight, 0, PF_UNKNOWN, PF_R24_UNORM_X8_TYPELESS);

    creatShadowPassConstantBuffer();
}

void FRenderer::updateShadowPass()
{
    mRHI->BeginEvent("ShadowDepth");
    //update light view
    updateShadowPassConstantBuffer();

    mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mShadowMap->Width), static_cast<float>(mShadowMap->Height), 1.0f);
    mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mShadowMap->Width), static_cast<float>(mShadowMap->Height));

    mRHI->SetRenderTarget(mShadowMap);

    const FRHITransitionInfo infoDepthBegin(mShadowMap->DepthStencilTarget, ACCESS_GENERIC_READ, ACCESS_DEPTH_WRITE);
    mRHI->TransitionResource(infoDepthBegin);

    const FVector4 clearColor(0.5f, 0.5f, 0.5f, 1.0f);
    mRHI->Clear(false, clearColor, true, 1, true, 0);

    const std::vector<FRenderProxy*>& renderProxys = mScene->GetRenderProxys();
    for (auto it = renderProxys.begin(); it != renderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;

        mRHI->BeginEvent(renderProxy->DebugName.c_str());

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = renderProxy->Material->VertexShaderShadow;
        info.PixelShader = nullptr;
        info.VertexLayout = &renderProxy->VertexLayout;
        info.DepthStencilState.bEnableDepthWrite = true;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(renderProxy->VertexBuffer);
        mRHI->SetIndexBuffer(renderProxy->IndexBuffer);
        mRHI->SetConstantBuffer(renderProxy->ConstantBuffer, 0);
        mRHI->SetConstantBuffer(mShadowPassConstantBuffer, 1);

        mRHI->DrawIndexedInstanced(renderProxy->IndexCountPerInstance, renderProxy->InstanceCount, renderProxy->StartIndexLocation, renderProxy->BaseVertexLocation, renderProxy->StartInstanceLocation);

        mRHI->EndEvent();
    }

    const FRHITransitionInfo infoDepthEnd(mShadowMap->DepthStencilTarget, ACCESS_DEPTH_WRITE, ACCESS_GENERIC_READ);
    mRHI->TransitionResource(infoDepthEnd);

    mRHI->EndEvent();

}

void FRenderer::unInitShadowPass()
{
    mShadowMap->UnInit();
    delete mShadowMap;
    mShadowMap = nullptr;

    mShadowPassConstantBuffer->UnInit();
    delete mShadowPassConstantBuffer;
    mShadowPassConstantBuffer = nullptr;

}

void FRenderer::creatShadowPassConstantBuffer()
{
    FShadowPassConstant shadowConstant;
    _createShadowPassConstant(shadowConstant);
    mShadowPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FShadowPassConstant), (uint8*)&shadowConstant);
}

void FRenderer::updateShadowPassConstantBuffer()
{
    FShadowPassConstant shadowConstant;
    _createShadowPassConstant(shadowConstant);
    mRHI->UpdateConstantBuffer(mShadowPassConstantBuffer, sizeof(FShadowPassConstant), (uint8*)&shadowConstant);
}

void FRenderer::_createShadowPassConstant(FShadowPassConstant& constant)
{
    FDirectionalLight* light = mScene->GetDirectionalLight();
    //should use negative value of camera direction in shader
    const float sceneBoundsRadius = 2000.0f;
    const FVector3 sceneBoundsCenter(0.0f, 0.0f, 0.0f);
    FVector3 lightDir = light->Direction;
    FVector3 lightPos = -sceneBoundsRadius * lightDir * 0.8f;
    FVector3 targetPos = sceneBoundsCenter;
    FVector3 lightUp(0.0f, 0.0f, 1.0f);
    FMatrix4x4 lightView;
    ConstructMatrixLookAtLH(lightView, lightPos, targetPos, lightUp);

    //transform bounding sphere to light space.
    FVector4 sphereCenterLightSpace;
    FVector4 bounds(sceneBoundsCenter.x(), sceneBoundsCenter.y(), sceneBoundsCenter.z(), 0.0f);
    sphereCenterLightSpace = lightView * bounds;

    //orthogonal frustum in light space encloses scene
    float l = sphereCenterLightSpace.x() - sceneBoundsRadius;
    float b = sphereCenterLightSpace.z() - sceneBoundsRadius;
    float n = sphereCenterLightSpace.y() - sceneBoundsRadius;
    float r = sphereCenterLightSpace.x() + sceneBoundsRadius;
    float t = sphereCenterLightSpace.z() + sceneBoundsRadius;
    float f = sphereCenterLightSpace.y() + sceneBoundsRadius;

    FMatrix4x4 lightProj;
    ConstructMatrixOrthoOffCenterLH(lightProj, l, r, b, t, n, f);

    FMatrix4x4 transformNDC;
    transformNDC << 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f;

    mShadowTransform = lightView * lightProj * transformNDC;

    constant.View = lightView;
    constant.Proj = lightProj;
    constant.ViewProj = lightView * lightProj;
}

void FRenderer::initSceneColorPass()
{
    //create shadow depth render target
    mSceneColor = mRHI->CreateRenderTarget(TSingleton<FConfigManager>::GetInstance().WindowWidth,
        TSingleton<FConfigManager>::GetInstance().WindowHeight, 1, PF_R16G16B16A16_FLOAT, PF_D24_UNORM_S8_UINT);

    createSceneColorPassConstantBuffer();
}

void FRenderer::updateSceneColorPass()
{
    mRHI->BeginEvent("SceneColorPass");

    mRHI->SetRenderTarget(mSceneColor);

    const FRHITransitionInfo infoRenderTargetBegin(mSceneColor->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
    mRHI->TransitionResource(infoRenderTargetBegin);
    const FRHITransitionInfo infoDepthStencilBegin(mSceneColor->DepthStencilTarget, ACCESS_GENERIC_READ, ACCESS_DEPTH_WRITE);
    mRHI->TransitionResource(infoDepthStencilBegin);

    const FVector4 clearColor(0.5f, 0.5f, 0.5f, 1.0f);
    mRHI->Clear(true, clearColor, true, 1, true, 0);

    mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mRenderTarget->Width), static_cast<float>(mRenderTarget->Height), 1.0f);
    mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mRenderTarget->Width), static_cast<float>(mRenderTarget->Height));

    updateSceneColorPassConstantBuffer();

    const std::vector<FRenderProxy*>& renderProxys = mScene->GetRenderProxys();
    for (auto it = renderProxys.begin(); it != renderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;

        mRHI->BeginEvent(renderProxy->DebugName.c_str());

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = renderProxy->Material->VertexShader;
        info.PixelShader = renderProxy->Material->PixelShader;
        info.VertexLayout = &renderProxy->VertexLayout;
        info.DepthStencilState.bEnableDepthWrite = true;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(renderProxy->VertexBuffer);
        mRHI->SetIndexBuffer(renderProxy->IndexBuffer);
        mRHI->SetConstantBuffer(renderProxy->ConstantBuffer, 0);
        mRHI->SetConstantBuffer(mSceneColorPassConstantBuffer, 1);
        mRHI->SetTexture2D(renderProxy->Material->BaseColor, 0);
        mRHI->SetTexture2D(mShadowMap->DepthStencilTarget, 1);

        mRHI->DrawIndexedInstanced(renderProxy->IndexCountPerInstance, renderProxy->InstanceCount, renderProxy->StartIndexLocation, renderProxy->BaseVertexLocation, renderProxy->StartInstanceLocation);

        mRHI->EndEvent();
    }

    const FRHITransitionInfo infoRederTargetEnd(mSceneColor->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
    mRHI->TransitionResource(infoRederTargetEnd);
    const FRHITransitionInfo infoDepthStencilEnd(mSceneColor->DepthStencilTarget, ACCESS_DEPTH_WRITE, ACCESS_GENERIC_READ);
    mRHI->TransitionResource(infoDepthStencilEnd);

    mRHI->EndEvent();
}

void FRenderer::unInitSceneColorPass()
{
    mSceneColor->UnInit();
    delete mSceneColor;
    mSceneColor = nullptr;

    mSceneColorPassConstantBuffer->UnInit();
    delete mSceneColorPassConstantBuffer;
    mSceneColorPassConstantBuffer = nullptr;
}

void FRenderer::createSceneColorPassConstantBuffer()
{
    //create pass constant buffer
    FSceneColorPassConstant mainConstant;
    _createSceneColorPassConstant(mainConstant);
    mSceneColorPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FSceneColorPassConstant), (uint8*)&mainConstant);
}

void FRenderer::updateSceneColorPassConstantBuffer()
{
    FSceneColorPassConstant mainConstant;
    _createSceneColorPassConstant(mainConstant);
    mRHI->UpdateConstantBuffer(mSceneColorPassConstantBuffer, sizeof(FSceneColorPassConstant), (uint8*)&mainConstant);
}

void FRenderer::_createSceneColorPassConstant(FSceneColorPassConstant& constant)
{
    FMatrix4x4 viewMatrix;
    ConstructMatrixLookRight(viewMatrix, mView->Position, mView->Look, mView->Right);

    FMatrix4x4 projectionMatrix;
    const float fovX = mView->FOV / 180.0f * 3.1415926f;
    const float aspectRatio = mView->AspectRatio;
    const float nearPlane = 1.0f;
    const float farPlane = 1000000.0f;
    ConstructMatrixPerspectiveFovXLH(projectionMatrix, fovX, aspectRatio, nearPlane, farPlane);

    constant.View = viewMatrix;
    constant.Proj = projectionMatrix;
    constant.ViewProj = viewMatrix * projectionMatrix;
    constant.ShadowTransform = mShadowTransform;
    constant.CameraPos = mView->Position;
    constant.CameraDir = mView->Look;

    FDirectionalLight* light = mScene->GetDirectionalLight();
    //should use negative value of camera direction in shader
    constant.DirectionalLightDir = -light->Direction;
    constant.DirectionalLightColor = FVector3(light->Color.x(), light->Color.y(), light->Color.z());
    const int32 shadowMapWidth = TSingleton<FConfigManager>::GetInstance().ShadowMapWidth;
    const int32 shadowMapHeight = TSingleton<FConfigManager>::GetInstance().ShadowMapHeight;
    constant.InvShadowMapSize = FVector2(1.0f / (shadowMapWidth), 1.0f / (shadowMapHeight));
}

void FRenderer::initPostProcess()
{
    mPostProcessing = new FPostProcessing(mRHI, mSceneColor, mRenderTarget);

    mPostProcessing->Init();
}

void FRenderer::updatePostProcess()
{
    mPostProcessing->Draw();
}

void FRenderer::unInitPostProcess()
{
    mPostProcessing->UnInit();
    delete mPostProcessing;
    mPostProcessing = nullptr;
}
