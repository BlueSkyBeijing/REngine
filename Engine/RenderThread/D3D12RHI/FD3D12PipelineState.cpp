#include "FD3D12PipelineState.h"
#include "FD3D12RHI.h"

FD3D12PipelineState::FD3D12PipelineState()
{
}

FD3D12PipelineState::~FD3D12PipelineState()
{
}

void FD3D12PipelineState::Init()
{
    TSingleton<FD3D12RHIManager>::GetInstance().GetRootDevice()->CreatePipelineState(this);

}

void FD3D12PipelineState::UnInit()
{
}
