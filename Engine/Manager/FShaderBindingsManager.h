#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

struct FRHIShaderRegisterCounts
{
    char SamplerCount;
    char ConstantBufferCount;
    char ShaderResourceCount;
    char UnorderedAccessCount;
};

class FShaderBindingsManager
{
    friend class TSingleton<FShaderBindingsManager>;

public:
    void Init();
    void UnInit();

    FRHIShaderBindings* GetShaderBindings();

private:
    FShaderBindingsManager();
    virtual ~FShaderBindingsManager();

private:
    std::map<int32, FRHIShaderBindings*> mShaderBindingss;
};
