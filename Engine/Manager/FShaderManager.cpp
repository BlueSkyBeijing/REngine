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

    const uint64 hashValue = haskShaderInfo(shaderInfo);

    auto iter = mShader.find(hashValue);
    if (iter == mShader.end())
    {
        FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

        shader = rhi->CreateShader(shaderInfo);

        mShader.insert(std::make_pair(hashValue, shader));
    }
    else
    {
        shader = iter->second;
    }

    return shader;
}

FRHIShader* FShaderManager::GetShader(const FShaderInfo& shaderInfo)
{
    const uint64 hashValue = haskShaderInfo(shaderInfo);

    auto iter = mShader.find(hashValue);
    if (iter != mShader.end())
    {
        return iter->second;
    }

    return nullptr;
}

uint64 FShaderManager::haskShaderInfo(const FShaderInfo& shaderInfo)
{
    int32 memSize = 0;

    memSize += static_cast<int32>(shaderInfo.FilePathName.size()) * 2;
    memSize += static_cast<int32>(shaderInfo.EnterPoint.size());
    memSize += static_cast<int32>(shaderInfo.Target.size());

    for (auto iter = shaderInfo.Defines.begin(); iter != shaderInfo.Defines.end(); iter++)
    {
        const std::string& define = iter->first;
        const std::string& defineValue = iter->second;

        memSize += static_cast<int32>(define.size());
        memSize += static_cast<int32>(defineValue.size());
    }

    char* info = new char[memSize];
    char* infoStart = info;

    memcpy(info, shaderInfo.FilePathName.c_str(), shaderInfo.FilePathName.size() * 2);
    info += static_cast<int32>(shaderInfo.FilePathName.size()) * 2;
    memcpy(info, shaderInfo.EnterPoint.c_str(), shaderInfo.EnterPoint.size());
    info += static_cast<int32>(shaderInfo.EnterPoint.size());
    memcpy(info, shaderInfo.Target.c_str(), shaderInfo.Target.size());
    info += static_cast<int32>(shaderInfo.Target.size());

    for (auto iter = shaderInfo.Defines.begin(); iter != shaderInfo.Defines.end(); iter++)
    {
        const std::string& define = iter->first;
        const std::string& defineValue = iter->second;

        memcpy(info, define.c_str(), define.size());
        info += static_cast<int32>(define.size());
        memcpy(info, defineValue.c_str(), defineValue.size());
        info += static_cast<int32>(defineValue.size());
    }

    const uint64 hashValue = HashMemory((const void*)info, memSize);

    delete[] infoStart;
    infoStart = nullptr;
    info = nullptr;

    return hashValue;
}