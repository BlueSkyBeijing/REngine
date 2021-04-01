#include "FShaderBindingsManager.h"
#include "d3dx12.h"
#include "FRHI.h"
#include "FRHIShaderBindings.h"

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

    FRHIShaderBindings* rootSignature = new FRHIShaderBindings;
    rootSignature->Init();

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
