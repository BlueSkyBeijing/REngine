#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"

class FD3D12Resource : public FRHIResource
{
public:
    FD3D12Resource();
    ~FD3D12Resource();

    virtual void Init() {}
    virtual void UnInit() {}

private:

};

class FD3D12SamplerState : public FRHISamplerState
{
public:
    FD3D12SamplerState();
    ~FD3D12SamplerState();

    D3D12_CPU_DESCRIPTOR_HANDLE Descriptor;
    uint32 DescriptorHeapIndex;
    const uint16 ID;

};

class FD3D12RasterizerState : public FRHIRasterizerState
{
public:
    D3D12_RASTERIZER_DESC Desc;
};

class FD3D12DepthStencilState : public FRHIDepthStencilState
{
public:

    D3D12_DEPTH_STENCIL_DESC1 Desc;
};

class FD3D12BlendState : public FRHIBlendState
{
public:

    D3D12_BLEND_DESC Desc;

};
