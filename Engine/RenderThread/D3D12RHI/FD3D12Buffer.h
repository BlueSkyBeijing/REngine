#pragma once
#include "FD3D12Resource.h"
#include "FD3D12Buffer.h"
#include "FD3D12Vertex.h"

#include <vector>
#include <wrl/client.h>
#include "d3d12.h"

class FD3D12Buffer : public FD3D12Resource
{
public:
	FD3D12Buffer();
	~FD3D12Buffer();

    virtual void Init() {}
    virtual void Uninit() {}

private:

};

class FD3D12VertexBuffer : public FD3D12Buffer
{
    friend class FD3D12RHI;
public:
    FD3D12VertexBuffer();
    ~FD3D12VertexBuffer();

public:
    virtual void Init() override;
    virtual void Uninit() override;

public:
    std::vector<FStaticMeshVertex> Vertexes;
    FD3D12VertexLayout VertexLayout;

private:

    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
    Microsoft::WRL::ComPtr <ID3D12Resource> mVertexBuffer;
};

class FD3D12IndexBuffer : public FD3D12Buffer
{
    friend class FD3D12RHI;

public:
    FD3D12IndexBuffer();
    ~FD3D12IndexBuffer();

public:
    virtual void Init() override;
    virtual void Uninit() override;

public:
    std::vector<uint16> Indexes;

private:
    D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
    Microsoft::WRL::ComPtr <ID3D12Resource> mIndexBuffer;

};

template<typename TBufferStruct>
class FD3D12ConstantBuffer : public FD3D12Buffer
{
    friend class FD3D12RHI;

public:
    FD3D12ConstantBuffer();
    ~FD3D12ConstantBuffer();

    int32 Slot;
public:
    virtual void Init() override;
    virtual void Uninit() override;

    TBufferStruct BufferStruct;
private:
    Microsoft::WRL::ComPtr <ID3D12Resource> mConstantBuffer;
};

#include "FD3D12Buffer.inl"