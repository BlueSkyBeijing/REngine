#include "PrecompiledHeader.h"

#include "FShaderManager.h"
#include "FRHI.h"
#include "FRHIShader.h"
#include "Utility.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRenderThread.h"

FShaderManager::FShaderManager()
{
}

FShaderManager::~FShaderManager()
{
}

void FShaderManager::Init()
{
}

void FShaderManager::UnInit()
{
    for (auto it = mShader.begin(); it != mShader.end(); it++)
    {
        FRHIShader* shader = it->second;
        delete shader;
    }

    mShader.clear();
}

FRHIShader* FShaderManager::CreateShader(const FShaderInfo& shaderInfo)
{
    FRHIShader* shader = nullptr;

    const uint64 hashValue = HashMemory((const void*)&shaderInfo, sizeof(FShaderInfo));

    auto iter = mShader.find(hashValue);
    if (iter == mShader.end())
    {
        FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

        shader = rhi->CreateShader(shaderInfo);

        mShader.insert(std::make_pair(hashValue, shader));
    }

    return shader;
}

FRHIShader* FShaderManager::GetShader(const FShaderInfo& shaderInfo)
{
    const uint64 hashValue = HashMemory((const void*)&shaderInfo, sizeof(FShaderInfo));

    auto iter = mShader.find(hashValue);
    if (iter != mShader.end())
    {
        return iter->second;
    }

    return nullptr;
}
