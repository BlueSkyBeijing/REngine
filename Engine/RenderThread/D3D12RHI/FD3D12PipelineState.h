#pragma once
#include "Prerequisite.h"

#include "FRHIPipelineState.h"


class FD3D12PipelineState : public FRHIPipelineState
{
    friend class FD3D12RHI;

public:
    FD3D12PipelineState();
    virtual ~FD3D12PipelineState();

public:
    void Init();
    void UnInit();

private:
    Microsoft::WRL::ComPtr <ID3D12PipelineState> mDX12PipleLineState;

};
