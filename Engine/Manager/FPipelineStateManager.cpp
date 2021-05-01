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

#include "CityHash.h"


FPipelineStateInfo::FPipelineStateInfo()
{
}

FPipelineStateInfo::~FPipelineStateInfo()
{
}

FPipelineStateManager::FPipelineStateManager()
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
    for (auto iter = mPipelineStates.begin(); iter != mPipelineStates.end(); iter++)
    {
        FRHIPipelineState* pipelineState = iter->second;
        pipelineState->UnInit();
        delete pipelineState;
    }

    mPipelineStates.clear();
}

FRHIPipelineState* FPipelineStateManager::CreatePipleLineState(const FPipelineStateInfo& info)
{
    FRHIPipelineState* pipelineState = nullptr;

    const uint64 hashValue = hashPipelineState((const void*)&info, sizeof(FPipelineStateInfo));

    auto iter = mPipelineStates.find(hashValue);
    if (iter == mPipelineStates.end())
    {
        FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();


        pipelineState = rhi->CreatePipelineState(info);

        mPipelineStates.insert(std::make_pair(hashValue, pipelineState));
    }

    return pipelineState;

}

FRHIPipelineState* FPipelineStateManager::GetPipleLineState(const FPipelineStateInfo& info)
{
    const uint64 hashValue = hashPipelineState((const void*)&info, sizeof(FPipelineStateInfo));

    auto iter = mPipelineStates.find(hashValue);
    if (iter != mPipelineStates.end())
    {
        return iter->second;
    }

    return nullptr;
}

uint64  FPipelineStateManager::hashPipelineState(const void* Data, int32 NumBytes)
{
    return CityHash64((const char*)Data, NumBytes);
}
