#pragma once
#include "TSingleton.h"
#include <wrl/client.h>
#include "d3d12.h"
#include "Utility.h"
#include <map>

class FRootSignature;

struct FShaderRegisterCounts
{
    char SamplerCount;
    char ConstantBufferCount;
    char ShaderResourceCount;
    char UnorderedAccessCount;
};

class FRootSignatureManager : TSingleton<FRootSignatureManager>
{
public:
    FRootSignatureManager();
    ~FRootSignatureManager();

    void Init();
    void UnInit();

    FRootSignature* GetOrCreateRootSignature();

private:
	std::map<int32, FRootSignature*> mRootSignatures;
};
