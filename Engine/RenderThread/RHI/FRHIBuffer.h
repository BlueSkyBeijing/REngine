#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"
#include "FRHIBuffer.h"
#include "FRHIVertex.h"

class FRHIBuffer : public FRHIResource
{
public:
    FRHIBuffer();
    virtual~FRHIBuffer() override;

public:
    virtual void Init() {}
    virtual void Uninit() {}

private:

};

class FRHIVertexBuffer : public FRHIBuffer
{
public:
    FRHIVertexBuffer();
    virtual ~FRHIVertexBuffer() override;

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
public:
    FRHIIndexBuffer();
    virtual ~FRHIIndexBuffer() override;

public:
    virtual void Init() override;
    virtual void Uninit() override;

public:
    std::vector<uint16> Indexes;

private:

};

class FRHIConstantBuffer : public FRHIBuffer
{
    friend class FRHI;

public:
    FRHIConstantBuffer();
    virtual ~FRHIConstantBuffer() override;

public:
    virtual void Init() override;
    virtual void Uninit() override;

private:
    uint8* mData;
};
