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

TSharedPtr<FRHIShaderBindings> FShaderBindingsManager::GetShaderBindings()
{
    TMap<int32, TSharedPtr<FRHIShaderBindings>>::iterator it = mShaderBindingss.begin();
    if (it != mShaderBindingss.end())
    {
        return it->second;
    }

    return nullptr;
}


void FShaderBindingsManager::Init()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

    TSharedPtr<FRHIShaderBindings> rootSignature(rhi->CreateShaderBindings());

    mShaderBindingss.insert(std::make_pair(0, rootSignature));
}


void FShaderBindingsManager::UnInit()
{
    TMap<int32, TSharedPtr<FRHIShaderBindings>>::iterator it = mShaderBindingss.begin();
    if (it != mShaderBindingss.end())
    {
        if (it->second != nullptr)
        {
            TSharedPtr<FRHIShaderBindings> rootSignature = it->second;;
            rootSignature->UnInit();
        }
    }

    mShaderBindingss.clear();
}
