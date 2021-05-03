#pragma once
#include "Prerequisite.h"

#include "FD3D12Resource.h"
#include "FD3D12Buffer.h"
#include "FD3D12Vertex.h"
#include "FRHIBuffer.h"


class FD3D12VertexBuffer : public FRHIVertexBuffer, public FD3D12Resource
{
    friend class FD3D12RHI;
public:
    FD3D12VertexBuffer();
    virtual ~FD3D12VertexBuffer() override;

public:
    virtual void Init() override;
    virtual void Uninit() override;

private:

    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
    Microsoft::WRL::ComPtr <ID3D12Resource> mVertexBuffer;
};

class FD3D12IndexBuffer : public FRHIIndexBuffer, public FD3D12Resource
{
    friend class FD3D12RHI;

public:
    FD3D12IndexBuffer();
    virtual ~FD3D12IndexBuffer() override;

public:
    virtual void Init() override;
    virtual void Uninit() override;

private:
    D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
    Microsoft::WRL::ComPtr <ID3D12Resource> mIndexBuffer;

};

class FD3D12ConstantBuffer : public FRHIConstantBuffer, public FD3D12Resource
{
    friend class FD3D12RHI;

public:
    FD3D12ConstantBuffer();
    virtual ~FD3D12ConstantBuffer() override;

public:
    virtual void Init() override;
    virtual void Uninit() override;

private:
    Microsoft::WRL::ComPtr <ID3D12Resource> mConstantBuffer;
};
