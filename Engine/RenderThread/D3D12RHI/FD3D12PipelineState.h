#pragma once
#include "FRHIPipelineState.h"

#include <d3d12.h>
#include <wrl/client.h>

class FD3D12ShaderBindings;
class FD3D12Shader;
class FD3D12VertexLayout;

class FD3D12PipelineState : FRHIPipelineState
{
    friend class FD3D12RHI;

public:
	FD3D12PipelineState();
	~FD3D12PipelineState();

    void Init();
    void UnInit();

private:
    Microsoft::WRL::ComPtr <ID3D12PipelineState> mDX12PipleLineState;

};
