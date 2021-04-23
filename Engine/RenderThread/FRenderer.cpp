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
#include "FLight.h"

FRenderer::FRenderer(FRHIRenderWindow* renderWindow, FScene* scene, FView* view) :
    mRHI(TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI()),
    mRenderTarget(renderWindow),
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

    createMainPassConstantBuffer();
    mRHI->FlushCommandQueue();

    initShadow();
}

void FRenderer::UnInit()
{
    mRHI->FlushCommandQueue();

    unInitShadow();

    mMainPassConstantBuffer->UnInit();
    delete mMainPassConstantBuffer;
    mMainPassConstantBuffer = nullptr;

    mShadowPassConstantBuffer->UnInit();
    delete mShadowPassConstantBuffer;
    mShadowPassConstantBuffer = nullptr;

}

void FRenderer::preRender()
{
    mRHI->BeginCommmandList();

}

void FRenderer::Render()
{
    preRender();

    updateShadow();

    clear();

    initView();

    drawRenderables();

    postProcess();

    postRender();
}

void FRenderer::clear()
{
    mRHI->SetRenderTarget(mRenderTarget);
    const FRHITransitionInfo info(mRenderTarget, ACCESS_PRESENT, ACCESS_RENDER_TARGET);
    mRHI->Transition(info);

    const FVector4 clearColor(0.5f, 0.5f, 0.5f, 1.0f);
    mRHI->Clear(true, clearColor, true, 1, true, 0);
}

void FRenderer::initView()
{
    setViewPort();
    computeVisibility();
    updateMainPassConstantBuffer();
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
    FViewPort viewPort = { 0.0f, 0.0f, static_cast<float>(mRenderTarget->Width), static_cast<float>(mRenderTarget->Height), 0.0f, 1.0f };
    //scissor rectangle
    FRect scissorRect = { 0, 0, static_cast<LONG>(mRenderTarget->Width), static_cast<LONG>(mRenderTarget->Height) };

    mRHI->SetViewPort(viewPort);
    mRHI->SetSetScissor(scissorRect);
}

void FRenderer::drawRenderables()
{
    mRHI->BeginEvent("Main");
    const std::vector<FRenderProxy*>& renderProxys = mScene->GetRenderProxys();
    for (auto it = renderProxys.begin(); it != renderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;

        mRHI->BeginEvent(renderProxy->DebugName.c_str());

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(renderProxy);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(renderProxy->VertexBuffer);
        mRHI->SetIndexBuffer(renderProxy->IndexBuffer);
        mRHI->SetConstantBuffer(renderProxy->ConstantBuffer);
        mRHI->SetConstantBuffer(mMainPassConstantBuffer);

        mRHI->DrawIndexedInstanced(renderProxy->IndexCountPerInstance, renderProxy->InstanceCount, renderProxy->StartIndexLocation, renderProxy->BaseVertexLocation, renderProxy->StartInstanceLocation);

        mRHI->EndEvent();
    }
    mRHI->EndEvent();
}

void FRenderer::postProcess()
{
}

void FRenderer::postRender()
{
    const FRHITransitionInfo info(mRenderTarget, ACCESS_RENDER_TARGET, ACCESS_PRESENT);
    mRHI->Transition(info);

    mRHI->EndCommmandList();

    mRHI->ExecuteCommandList();
    mRHI->FlushCommandQueue();
}

void FRenderer::initShadow()
{
    //create shadow depth render target
    mShadowMap = mRHI->CreateRenderTarget(TSingleton<FConfigManager>::GetInstance().ShadowMapWidth,
        TSingleton<FConfigManager>::GetInstance().ShadowMapHeight, 0, PF_UNKNOWN, PF_R24_UNORM_X8_TYPELESS);

    creatShadowPassConstantBuffer();
}

void FRenderer::updateShadow()
{
    mRHI->BeginEvent("ShadowDepth");
    //update light view
    updateShadowPassConstantBuffer();

    //viewport
    FViewPort viewPort = { 0.0f, 0.0f, static_cast<float>(mShadowMap->Width), static_cast<float>(mShadowMap->Height), 0.0f, 1.0f };
    //scissor rectangle
    FRect scissorRect = { 0, 0, static_cast<LONG>(mShadowMap->Width), static_cast<LONG>(mShadowMap->Height) };

    mRHI->SetViewPort(viewPort);
    mRHI->SetSetScissor(scissorRect);

    //update shadow map
    mRHI->SetRenderTarget(mShadowMap);

    const FRHITransitionInfo infoBegin(mShadowMap, ACCESS_GENERIC_READ, ACCESS_DEPTH_WRITE);
    mRHI->Transition(infoBegin);

    const FVector4 clearColor(0.5f, 0.5f, 0.5f, 1.0f);
    mRHI->Clear(false, clearColor, true, 1, true, 0);

    const std::vector<FRenderProxy*>& renderProxys = mScene->GetRenderProxys();
    for (auto it = renderProxys.begin(); it != renderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;

        mRHI->BeginEvent(renderProxy->DebugName.c_str());

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineStateShadow(renderProxy);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(renderProxy->VertexBuffer);
        mRHI->SetIndexBuffer(renderProxy->IndexBuffer);
        mRHI->SetConstantBuffer(renderProxy->ConstantBuffer);
        mRHI->SetConstantBuffer(mShadowPassConstantBuffer);

        mRHI->DrawIndexedInstanced(renderProxy->IndexCountPerInstance, renderProxy->InstanceCount, renderProxy->StartIndexLocation, renderProxy->BaseVertexLocation, renderProxy->StartInstanceLocation);

        mRHI->EndEvent();
    }

    const FRHITransitionInfo infoEnd(mShadowMap, ACCESS_DEPTH_WRITE, ACCESS_GENERIC_READ);
    mRHI->Transition(infoEnd);

    mRHI->EndEvent();
}
void FRenderer::unInitShadow()
{
    delete mShadowMap;
    mShadowMap = nullptr;
}

void FRenderer::createMainPassConstantBuffer()
{
    //create pass constant buffer
    FMainPassConstant mainConstant;
    _createMainPassConstant(mainConstant);
    mMainPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FMainPassConstant), (uint8*)&mainConstant, 3);
}

void FRenderer::updateMainPassConstantBuffer()
{
    FMainPassConstant mainConstant;
    _createMainPassConstant(mainConstant);
    mRHI->UpdateConstantBuffer(mMainPassConstantBuffer, sizeof(FMainPassConstant), (uint8*)&mainConstant);
}

void FRenderer::creatShadowPassConstantBuffer()
{
    FShadowPassConstant shadowConstant;
    _createShadowPassConstant(shadowConstant);
    mShadowPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FShadowPassConstant), (uint8*)&shadowConstant, 3);
}

void FRenderer::updateShadowPassConstantBuffer()
{
    FShadowPassConstant shadowConstant;
    _createShadowPassConstant(shadowConstant);
    mRHI->UpdateConstantBuffer(mShadowPassConstantBuffer, sizeof(FShadowPassConstant), (uint8*)&shadowConstant);
}

void FRenderer::_createMainPassConstant(FMainPassConstant& constant)
{
    FMatrix4x4 viewMatrix;
    ConstructMatrixLookRight(viewMatrix, mView->Position, mView->Look, mView->Right);

    FMatrix4x4 projectionMatrix;
    const float fovY = mView->FOV;
    const float aspect = mView->AspectRatio;
    const float nearPlane = 1.0f;
    const float farPlane = 1000000.0f;
    ConstructMatrixPerspectiveFovLH(projectionMatrix, fovY, aspect, nearPlane, farPlane);

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

}

void FRenderer::_createShadowPassConstant(FShadowPassConstant& constant)
{
    FDirectionalLight* light = mScene->GetDirectionalLight();
    //should use negative value of camera direction in shader
    constant.DirectionalLightDir = -light->Direction;
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

    //transform NDC space [-1,+1]^2 to texture space [0,1]^2
    FMatrix4x4 transformNDC;

    transformNDC << 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f;

    mShadowTransform = lightView * lightProj * transformNDC;

    constant.View = lightView;
    constant.Proj = lightProj;
    constant.ViewProj = lightView * lightProj;
    constant.ShadowTransform = mShadowTransform;
}
