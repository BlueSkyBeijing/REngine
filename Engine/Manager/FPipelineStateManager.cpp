#include "PrecompiledHeader.h"

#include "FPipelineStateManager.h"
#include "FRHIPipelineState.h"
#include "FRenderProxy.h"
#include "FResourceManager.h"
#include "FRHIShaderBindings.h"
#include "FShaderBindingsManager.h"
#include "FMaterial.h"
#include "FRHIVertex.h"
#include "FRHIShaderBindings.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRHI.h"
#include "FRenderThread.h"

FPipelineStateManager::FPipelineStateManager() :
    mPipelineState(nullptr)
{
}

FPipelineStateManager::~FPipelineStateManager()
{
}

void FPipelineStateManager::Init()
{
}

void FPipelineStateManager::UnInit()
{
    if (mPipelineState != nullptr)
    {
        delete mPipelineState;
        mPipelineState = nullptr;
    }

    if (mPipelineStateShadow != nullptr)
    {
        delete mPipelineStateShadow;
        mPipelineStateShadow = nullptr;
    }

    if (mPipelineStateFullscreenQuad != nullptr)
    {
        delete mPipelineStateFullscreenQuad;
        mPipelineStateFullscreenQuad = nullptr;
    }
}

FRHIPipelineState* FPipelineStateManager::CreatePipleLineState(FRenderProxy* renderProxy)
{
    if (nullptr == mPipelineState)
    {
        FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetOrCreateRootSignature();

        mPipelineState = rhi->CreatePipelineState(shaderBindings, renderProxy->Material->VertexShader, renderProxy->Material->PixelShader, &renderProxy->VertexLayout);

        mPipelineStateShadow = rhi->CreatePipelineStateShadow(shaderBindings, renderProxy->Material->VertexShaderShadow, nullptr, &renderProxy->VertexLayout);
    }

    return mPipelineState;
}

FRHIPipelineState* FPipelineStateManager::CreatePipleLineState(FRHIShader* vertexShader, FRHIShader* pixelShader, FRHIVertexLayout* vertexLayout)
{
    if (nullptr == mPipelineStateFullscreenQuad)
    {
        FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetOrCreateRootSignature();

        mPipelineStateFullscreenQuad = rhi->CreatePipelineStateFullScreenQuad(shaderBindings, vertexShader, pixelShader, vertexLayout);
    }

    return mPipelineStateFullscreenQuad;
}


FRHIPipelineState* FPipelineStateManager::GetPipleLineState(FRenderProxy* renderProxy)
{
    return mPipelineState;
}

FRHIPipelineState* FPipelineStateManager::GetPipleLineStateShadow(FRenderProxy* renderProxy)
{
    return mPipelineStateShadow;
}

FRHIPipelineState* FPipelineStateManager::GetPipleLineStateFullscreenQuad()
{
    return mPipelineStateFullscreenQuad;
}
