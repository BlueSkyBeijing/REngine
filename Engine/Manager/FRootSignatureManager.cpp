#include "FRootSignatureManager.h"
#include "d3dx12.h"
#include "FDeviceManager.h"
#include "FDevice.h"
#include "FRootSignature.h"

FRootSignatureManager::FRootSignatureManager()
{
}

FRootSignatureManager::~FRootSignatureManager()
{
}
FRootSignature* FRootSignatureManager::GetOrCreateRootSignature()
{
    std::map<int32, FRootSignature*>::iterator it = mRootSignatures.begin();
    if (it != mRootSignatures.end())
    {
        return it->second;
    }

    FRootSignature* rootSignature = new FRootSignature;
    rootSignature->Init();

    mRootSignatures.insert(std::make_pair(0, rootSignature));

    return rootSignature;
}


void FRootSignatureManager::Init()
{
}

void FRootSignatureManager::UnInit()
{
    std::map<int32, FRootSignature*>::iterator it = mRootSignatures.begin();
    if (it != mRootSignatures.end())
    {
        if (it->second != nullptr)
        {
            FRootSignature* rootSignature = it->second;;
            rootSignature->UnInit();
            delete rootSignature;
        }
    }

    mRootSignatures.clear();
}
