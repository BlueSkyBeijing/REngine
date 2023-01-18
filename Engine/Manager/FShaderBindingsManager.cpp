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

FRHIShaderBindings* FShaderBindingsManager::GetShaderBindings()
{
    TMap<int32, FRHIShaderBindings*>::iterator it = mShaderBindingss.begin();
    if (it != mShaderBindingss.end())
    {
        return it->second;
    }

    return nullptr;
}


void FShaderBindingsManager::Init()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

    FRHIShaderBindings* rootSignature = rhi->CreateShaderBindings();

    mShaderBindingss.insert(std::make_pair(0, rootSignature));
}


void FShaderBindingsManager::UnInit()
{
    TMap<int32, FRHIShaderBindings*>::iterator it = mShaderBindingss.begin();
    if (it != mShaderBindingss.end())
    {
        if (it->second != nullptr)
        {
            FRHIShaderBindings* rootSignature = it->second;;
            rootSignature->UnInit();
            delete rootSignature;
        }
    }

    mShaderBindingss.clear();
}
