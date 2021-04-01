#include "FD3D12ShaderBindings.h"
#include "FD3D12RHI.h"


FD3D12ShaderBindings::FD3D12ShaderBindings()
{
}

FD3D12ShaderBindings::~FD3D12ShaderBindings()
{
}

void FD3D12ShaderBindings::Init()
{
    TSingleton<FD3D12RHIManager>::GetInstance().GetRootDevice()->CreateRootSignature(this);

}

void FD3D12ShaderBindings::UnInit()
{
}
