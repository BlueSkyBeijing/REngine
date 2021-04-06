#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FRHIShaderBindings;

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

    FRHIShaderBindings* GetOrCreateRootSignature();

private:
	std::map<int32, FRHIShaderBindings*> mRootSignatures;
};
