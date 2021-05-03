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

class FShaderBindingsManager : TSingleton<FShaderBindingsManager>
{
public:
    FShaderBindingsManager();
    ~FShaderBindingsManager();

    void Init();
    void UnInit();

    FRHIShaderBindings* GetRootSignature();

private:
    std::map<int32, FRHIShaderBindings*> mRootSignatures;
};
