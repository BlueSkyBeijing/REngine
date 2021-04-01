#pragma once
#include <d3d12.h>
#include <wrl/client.h>


class FRootSignature
{
    friend class FDevice;

public:
	FRootSignature();
	~FRootSignature();

    void Init();
    void UnInit();

private:
    Microsoft::WRL::ComPtr <ID3D12RootSignature> mDX12RootSignature;
};
