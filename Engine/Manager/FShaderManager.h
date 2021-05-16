#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"


class FShaderManager
{
    friend class TSingleton<FShaderManager>;

public:
    void Init();
    void UnInit();

    FRHIShader* CreateShader(const FShaderInfo& shaderInfo);
    FRHIShader* GetShader(const FShaderInfo& shaderInfo);

private:
    FShaderManager();
    virtual ~FShaderManager();

private:
    std::map<uint64, FRHIShader*> mShader;

};
