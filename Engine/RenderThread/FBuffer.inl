#pragma once
#include "FDeviceManager.h"
#include "FDevice.h"

template<typename TBufferStruct>
FConstantBuffer<TBufferStruct>::FConstantBuffer()
{
}

template<typename TBufferStruct>
FConstantBuffer<TBufferStruct>::~FConstantBuffer()
{
}

template<typename TBufferStruct>
void FConstantBuffer<TBufferStruct>::Init()
{
    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->CreateConstantBuffer(this);
}

template<typename TBufferStruct>
void FConstantBuffer<TBufferStruct>::Uninit()
{
}

