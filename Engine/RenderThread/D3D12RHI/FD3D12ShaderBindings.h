#pragma once
#include <d3d12.h>
#include <wrl/client.h>


class FD3D12ShaderBindings : public FRHIShaderBindings
{
    friend class FD3D12RHI;

public:
	FD3D12ShaderBindings();
	~FD3D12ShaderBindings();

    void Init();
    void UnInit();

private:
    Microsoft::WRL::ComPtr <ID3D12RootSignature> mDX12RootSignature;
};
