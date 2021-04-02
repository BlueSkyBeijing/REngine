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

FPipelineStateManager::FPipelineStateManager():
    mPipelineState(nullptr)
{
}

FPipelineStateManager::~FPipelineStateManager()
{
}

FRHIPipelineState* FPipelineStateManager::GetOrCreatePipleLineState(FRenderProxy* renderProxy)
{
    if (nullptr == mPipelineState)
    {
        FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetOrCreateRootSignature();

        mPipelineState  = rhi->CreatePipelineState(shaderBindings, renderProxy->Material->VertexShader, renderProxy->Material->PixelShader, &renderProxy->VertexLayout);
    }

    return mPipelineState;
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
}
