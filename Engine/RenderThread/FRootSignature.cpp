#include "FRootSignature.h"
#include "FDeviceManager.h"
#include "FDevice.h"


FRootSignature::FRootSignature()
{
}

FRootSignature::~FRootSignature()
{
}

void FRootSignature::Init()
{
    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->CreateRootSignature(this);

}

void FRootSignature::UnInit()
{
}
