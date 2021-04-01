#include "FPipelineStateManager.h"
#include "FPipelineState.h"
#include "FRenderProxy.h"
#include "FResourceManager.h"
#include "FRootSignature.h"
#include "FRootSignatureManager.h"
#include "FMaterial.h"
#include "FVertex.h"

FPipelineStateManager::FPipelineStateManager():
    mPipelineState(nullptr)
{
}

FPipelineStateManager::~FPipelineStateManager()
{
}

FPipelineState* FPipelineStateManager::GetOrCreatePipleLineState(FRenderProxy* renderProxy)
{
    if (nullptr == mPipelineState)
    {
        mPipelineState = new FPipelineState;
        FRootSignature* rootSignature = TSingleton<FRootSignatureManager>::GetInstance().GetOrCreateRootSignature();

        mPipelineState->VertexShader = renderProxy->Material->VertexShader;
        mPipelineState->PixelShader = renderProxy->Material->PixelShader;
        mPipelineState->RootSignature = rootSignature;
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
