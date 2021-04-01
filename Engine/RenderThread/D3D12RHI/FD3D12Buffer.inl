#pragma once
#include "FD3D12RHI.h"

template<typename TBufferStruct>
FD3D12ConstantBuffer<TBufferStruct>::FD3D12ConstantBuffer()
{
}

template<typename TBufferStruct>
FD3D12ConstantBuffer<TBufferStruct>::~FD3D12ConstantBuffer()
{
}

template<typename TBufferStruct>
void FD3D12ConstantBuffer<TBufferStruct>::Init()
{
    TSingleton<FD3D12RHIManager>::GetInstance().GetRootDevice()->CreateConstantBuffer(this);
}

template<typename TBufferStruct>
void FD3D12ConstantBuffer<TBufferStruct>::Uninit()
{
}

