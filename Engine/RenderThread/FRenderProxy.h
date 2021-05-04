#pragma once
#include "Prerequisite.h"

#include "FRHIVertex.h"
#include "FRHIBuffer.h"


// render thread render element

struct FObjectConstant
{
    FMatrix4x4 World;
};

struct FRenderProxyInitializer
{
public:
    FRenderProxyInitializer();
    ~FRenderProxyInitializer();

    std::vector<FStaticMeshVertex> Vertexes;
    std::vector<uint16> Indexes;
    FRHIVertexLayout VertexLayout;
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;
    FMaterial* Material;
};

class FRenderProxy
{
public:
    FRenderProxy();
    virtual ~FRenderProxy();

    virtual void CreateRenderResource();
    virtual void ReleaseRenderResource();

public:
    FMaterial* Material;
    FRHIVertexBuffer* VertexBuffer;
    FRHIIndexBuffer* IndexBuffer;
    FRHIConstantBuffer* ConstantBuffer;
    uint32 IndexCountPerInstance;
    uint32 InstanceCount;
    uint32 StartIndexLocation;
    int32 BaseVertexLocation;
    uint32 StartInstanceLocation;
    FMatrix4x4 WorldMatrix;
    FRHIVertexLayout VertexLayout;
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;

    std::string DebugName;

protected:

};

class FStaticMeshRenderProxy : public FRenderProxy
{
public:
    FStaticMeshRenderProxy(const FRenderProxyInitializer& initializer);
    virtual ~FStaticMeshRenderProxy() override;

public:
    virtual void CreateRenderResource() override;
    virtual void ReleaseRenderResource() override;

private:
    FObjectConstant mObjectConstants;

    std::vector<FStaticMeshVertex> mVertexes;
    std::vector<uint16> mIndexes;
};
