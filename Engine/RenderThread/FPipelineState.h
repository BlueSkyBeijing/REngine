#pragma once
#include <d3d12.h>
#include <wrl/client.h>

class FRootSignature;
class FShader;
class FVertexLayout;

class FPipelineState
{
    friend class FDevice;

public:
	FPipelineState();
	~FPipelineState();

    void Init();
    void UnInit();

    FRootSignature* RootSignature;
    FShader* VertexShader;
    FShader* PixelShader;
    FVertexLayout* VertexLayout;

private:
    Microsoft::WRL::ComPtr <ID3D12PipelineState> mDX12PipleLineState;

};
