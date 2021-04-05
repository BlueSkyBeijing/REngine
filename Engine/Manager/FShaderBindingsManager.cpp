#include "PrecompiledHeader.h"
#include "FShaderBindingsManager.h"
#include "FRHI.h"
#include "FRHIShaderBindings.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRenderThread.h"

FShaderBindingsManager::FShaderBindingsManager()
{
}

FShaderBindingsManager::~FShaderBindingsManager()
{
}
FRHIShaderBindings* FShaderBindingsManager::GetOrCreateRootSignature()
{
    std::map<int32, FRHIShaderBindings*>::iterator it = mRootSignatures.begin();
    if (it != mRootSignatures.end())
    {
        return it->second;
    }

    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

    FRHIShaderBindings* rootSignature = rhi->CreateShaderBindings();

    mRootSignatures.insert(std::make_pair(0, rootSignature));

    return rootSignature;
}


void FShaderBindingsManager::Init()
{
}

void FShaderBindingsManager::UnInit()
{
    std::map<int32, FRHIShaderBindings*>::iterator it = mRootSignatures.begin();
    if (it != mRootSignatures.end())
    {
        if (it->second != nullptr)
        {
            FRHIShaderBindings* rootSignature = it->second;;
            rootSignature->UnInit();
            delete rootSignature;
        }
    }

    mRootSignatures.clear();
}
