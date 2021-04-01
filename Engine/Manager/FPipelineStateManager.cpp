#include "FPipelineStateManager.h"
#include "FRHIPipelineState.h"
#include "FRenderProxy.h"
#include "FResourceManager.h"
#include "FRHIShaderBindings.h"
#include "FShaderBindingsManager.h"
#include "FMaterial.h"
#include "FRHIVertex.h"

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
        mPipelineState = new FRHIPipelineState;
        FRHIShaderBindings* rootSignature = TSingleton<FShaderBindingsManager>::GetInstance().GetOrCreateRootSignature();

        mPipelineState->VertexShader = renderProxy->Material->VertexShader;
        mPipelineState->PixelShader = renderProxy->Material->PixelShader;
        mPipelineState->ShaderBindings = rootSignature;
        mPipelineState->VertexLayout = &renderProxy->VertexLayout;

        mPipelineState->Init();
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
        mPipelineState->UnInit();
        delete mPipelineState;
        mPipelineState = nullptr;
    }
}
