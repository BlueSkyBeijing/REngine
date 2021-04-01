#include "FShaderManager.h"
#include "FDeviceManager.h"
#include "FDevice.h"
#include "FShader.h"

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
        FShader* shader = it->second;
        delete shader;
    }

    mShader.clear();
}

FShader* FShaderManager::GetOrCreateShader(std::string& fileName, std::string& enterPoint, std::string& target)
{
    std::map<std::string, FShader*>::iterator it =  mShader.find(fileName);
    if (it != mShader.end())
    {
        return it->second;
    }

    FShader* shader = new FShader;
    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->CreateShader(shader);

    return nullptr;
}
