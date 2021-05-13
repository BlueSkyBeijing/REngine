#pragma once
#include "Prerequisite.h"

#include "FRHIVertex.h"
#include "FRHIBuffer.h"


// render thread render element

struct FObjectConstant
{
    FMatrix4x4 World;
};

struct SkeletonMeshConstant
{
    FMatrix4x4 World;
    FMatrix4x4 BoneTransforms[75];
};

struct FStaticMeshRenderProxyInitializer
{
public:
    FStaticMeshRenderProxyInitializer();
    ~FStaticMeshRenderProxyInitializer();

    std::vector<FStaticMeshVertex> Vertexes;
    std::vector<uint16> Indexes;
    FRHIVertexLayout VertexLayout;
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;
    FMaterial* Material;
};

struct FSkeletalMeshRenderProxyInitializer
{
public:
    FSkeletalMeshRenderProxyInitializer();
    ~FSkeletalMeshRenderProxyInitializer();

    std::vector<FSkeletalMeshVertex> Vertexes;
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
    FStaticMeshRenderProxy(const FStaticMeshRenderProxyInitializer& initializer);
    virtual ~FStaticMeshRenderProxy() override;

public:
    virtual void CreateRenderResource() override;
    virtual void ReleaseRenderResource() override;

private:
    FObjectConstant mObjectConstants;

    std::vector<FStaticMeshVertex> mVertexes;
    std::vector<uint16> mIndexes;
};

class FSkeletalMeshRenderProxy : public FRenderProxy
{
public:
    FSkeletalMeshRenderProxy(const FSkeletalMeshRenderProxyInitializer& initializer);
    virtual ~FSkeletalMeshRenderProxy() override;

public:
    virtual void CreateRenderResource() override;
    virtual void ReleaseRenderResource() override;

private:
    SkeletonMeshConstant mObjectConstants;

    std::vector<FSkeletalMeshVertex> mVertexes;
    std::vector<uint16> mIndexes;
};
