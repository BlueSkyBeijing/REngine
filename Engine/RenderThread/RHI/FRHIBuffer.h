#pragma once
#include "FRHIResource.h"
#include "FRHIBuffer.h"
#include "FRHIVertex.h"
#include <vector>

class FRHIBuffer : public FRHIResource
{
public:
	FRHIBuffer();
	~FRHIBuffer();

    virtual void Init() {}
    virtual void Uninit() {}

private:

};

class FRHIVertexBuffer : public FRHIBuffer
{
    friend class FRHI;
public:
    FRHIVertexBuffer();
    ~FRHIVertexBuffer();

public:
    virtual void Init() override;
    virtual void Uninit() override;

public:
    std::vector<FStaticMeshVertex> Vertexes;
    FRHIVertexLayout VertexLayout;

private:
};

class FRHIIndexBuffer : public FRHIBuffer
{
    friend class FRHI;

public:
    FRHIIndexBuffer();
    ~FRHIIndexBuffer();

public:
    virtual void Init() override;
    virtual void Uninit() override;

public:
    std::vector<uint16> Indexes;

private:

};

template<typename TBufferStruct>
class FRHIConstantBuffer : public FRHIBuffer
{
    friend class FRHI;

public:
    FRHIConstantBuffer();
    ~FRHIConstantBuffer();

    int32 Slot;
public:
    virtual void Init() override;
    virtual void Uninit() override;

    TBufferStruct BufferStruct;
private:
};

#include "FD3D12Buffer.inl"