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


FFullscreenQuad::FFullscreenQuad(FRHI* rhi) :
    mRHI(rhi)
{
}

FFullscreenQuad::~FFullscreenQuad()
{
}


void FFullscreenQuad::Init()
{
    const FScreenVertex vertexes[4] = { FScreenVertex(FVector3(-1.f, -1.f, 0.0f), FVector2(0.0f, 1.0f)),
        FScreenVertex(FVector3(1.f, -1.f, 0.0f), FVector2(1.0f, 1.0f)),
        FScreenVertex(FVector3(-1.f, 1.f, 0.0f), FVector2(0.0f, 0.0f)),
        FScreenVertex(FVector3(1.f, 1.f, 0.0f), FVector2(1.0f, 0.0f)) };
    const uint16 indexes[6] = { 0, 2, 1, 1, 2, 3 };

    VertexBuffer = mRHI->CreateVertexBuffer(sizeof(FScreenVertex), 4, (uint8*)vertexes);
    IndexBuffer = mRHI->CreateIndexBuffer(sizeof(uint16), 6, (uint8*)indexes);
}

void FFullscreenQuad::UnInit()
{
    delete VertexBuffer;
    VertexBuffer = nullptr;

    delete IndexBuffer;
    IndexBuffer = nullptr;
}

void FFullscreenQuad::Draw()
{
}

FRenderer::FRenderer(FRHIRenderWindow* renderWindow, FScene* scene, FView* view) :
    mRHI(TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI()),
    mRenderTarget(renderWindow),
    mScene(scene),
    mView(view),
    mFullscreenQuad(nullptr)
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

    mFullscreenQuad = new FFullscreenQuad(mRHI);
    mFullscreenQuad->Init();
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

    mFullscreenQuad->UnInit();
    delete mFullscreenQuad;
    mFullscreenQuad = nullptr;
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
    mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mRenderTarget->Width), static_cast<float>(mRenderTarget->Height), 1.0f);
    mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mRenderTarget->Width), static_cast<float>(mRenderTarget->Height));
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
        mRHI->SetConstantBuffer(renderProxy->ConstantBuffer, 1);
        mRHI->SetConstantBuffer(mMainPassConstantBuffer, 3);
        mRHI->SetTexture2D(renderProxy->Material->BaseColor, 0);

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

    mRenderTarget->PosInHeap = mShadowMap->PosInHeap;

    creatShadowPassConstantBuffer();
}

void FRenderer::updateShadow()
{
    mRHI->BeginEvent("ShadowDepth");
    //update light view
    updateShadowPassConstantBuffer();

    mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mShadowMap->Width), static_cast<float>(mShadowMap->Height), 1.0f);
    mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mShadowMap->Width), static_cast<float>(mShadowMap->Height));

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
        mRHI->SetConstantBuffer(renderProxy->ConstantBuffer, 1);
        mRHI->SetConstantBuffer(mShadowPassConstantBuffer, 3);

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
    mMainPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FMainPassConstant), (uint8*)&mainConstant);
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
    mShadowPassConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FShadowPassConstant), (uint8*)&shadowConstant);
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
