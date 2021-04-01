#pragma once
#include "TSingleton.h"
#include <wrl/client.h>
#include "d3d12.h"
#include "Utility.h"
#include <map>

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
