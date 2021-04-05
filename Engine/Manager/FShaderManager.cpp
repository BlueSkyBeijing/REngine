#include "PrecompiledHeader.h"
#include "FShaderManager.h"
#include "FRHI.h"
#include "FRHIShader.h"

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

FRHIShader* FShaderManager::GetOrCreateShader(std::string& fileName, std::string& enterPoint, std::string& target)
{
    std::map<std::string, FRHIShader*>::iterator it =  mShader.find(fileName);
    if (it != mShader.end())
    {
        return it->second;
    }

    return nullptr;
}
