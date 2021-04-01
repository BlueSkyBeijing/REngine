#pragma once
#include "FRenderResource.h"
#include "FBuffer.h"
#include "FVertex.h"
#include <vector>
#include <wrl/client.h>
#include "d3d12.h"

class FBuffer : public FRenderResource
{
public:
	FBuffer();
	~FBuffer();

    virtual void Init() {}
    virtual void Uninit() {}

private:

};

class FVertexBuffer : public FBuffer
{
    friend class FDevice;
public:
    FVertexBuffer();
    ~FVertexBuffer();

public:
    virtual void Init() override;
    virtual void Uninit() override;

public:
    std::vector<FStaticMeshVertex> Vertexes;
    FVertexLayout VertexLayout;

private:

    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
    Microsoft::WRL::ComPtr <ID3D12Resource> mVertexBuffer;
};

class FIndexBuffer : public FBuffer
{
    friend class FDevice;

public:
    FIndexBuffer();
    ~FIndexBuffer();

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
class FConstantBuffer : public FBuffer
{
    friend class FDevice;

public:
    FConstantBuffer();
    ~FConstantBuffer();

    int32 Slot;
public:
    virtual void Init() override;
    virtual void Uninit() override;

    TBufferStruct BufferStruct;
private:
    Microsoft::WRL::ComPtr <ID3D12Resource> mConstantBuffer;
};

#include "FBuffer.inl"