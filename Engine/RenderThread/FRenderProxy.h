#pragma once
#include "Prerequisite.h"

#include "FRHIVertex.h"
#include "FRHIBuffer.h"


// render thread render element

struct FObjectConstant
{
    FMatrix4x4 World;
    FVector4 BaseColorScale;
    FVector4 EmissiveColorScale;
    FVector4 SubsurfaceColorScale;

    float MetallicScale;
    float SpecularScale;
    float RoughnessScale;
    float OpacityScale;
};

struct FSkeletonMeshConstant
{
    FMatrix4x4 World;
    FMatrix4x4 BoneTransforms[75];
    FVector4 BaseColorScale;
    FVector4 EmissiveColorScale;
    FVector4 SubsurfaceColorScale;

    float MetallicScale;
    float SpecularScale;
    float RoughnessScale;
    float OpacityScale;
};

struct FStaticMeshRenderProxyInitializer
{
public:
    FStaticMeshRenderProxyInitializer();
    ~FStaticMeshRenderProxyInitializer();

    TArray<FStaticMeshVertex> Vertexes;
    TArray<uint32> Indexes;
    FRHIVertexLayout VertexLayout;
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;
    TArray<FMaterial*> Materials;
    TArray<FStaticMeshSection> mSections;
    TArray<FMeshBatch*> MeshBatchs;

};

struct FSkeletalMeshRenderProxyInitializer
{
public:
    FSkeletalMeshRenderProxyInitializer();
    ~FSkeletalMeshRenderProxyInitializer();

    TArray<FSkeletalMeshVertex> Vertexes;
    TArray<uint32> Indexes;
    FRHIVertexLayout VertexLayout;
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;
    TArray<FMaterial*> Materials;
    TArray<FStaticMeshSection> mSections;
    TArray<FMeshBatch*> MeshBatchs;

};

class FMeshBatch
{
public:
    FMeshBatch();
    virtual ~FMeshBatch();

    virtual void CreateRenderResource();
    virtual void UpdateRenderResource();
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

    FString DebugName;

};


class FStaticMeshBatch : public FMeshBatch
{
public:
    FStaticMeshBatch();
    virtual ~FStaticMeshBatch();

public:
    FObjectConstant ObjectConstants;

};

class FSkeletalMeshBatch : public FMeshBatch
{
public:
    FSkeletalMeshBatch();
    virtual ~FSkeletalMeshBatch();

public:
    FSkeletonMeshConstant ObjectConstants;
    TArray<FMatrix4x4> BoneFinalTransforms;

};

class FRenderProxy
{
public:
    FRenderProxy();
    virtual ~FRenderProxy();

    virtual void CreateRenderResource();
    virtual void UpdateRenderResource();
    virtual void ReleaseRenderResource();

public:
    TArray<FMaterial*> Materials;
    FRHIVertexBuffer* VertexBuffer;
    FRHIIndexBuffer* IndexBuffer;
    FRHIVertexLayout VertexLayout;
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;

    TArray<FMeshBatch*> MeshBatchs;

    FString DebugName;

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
    TArray<FStaticMeshVertex> mVertexes;
    TArray<uint32> mIndexes;
    TArray<FStaticMeshSection> mSections;

};

class FSkeletalMeshRenderProxy : public FRenderProxy
{
public:
    FSkeletalMeshRenderProxy(const FSkeletalMeshRenderProxyInitializer& initializer);
    virtual ~FSkeletalMeshRenderProxy() override;

public:
    virtual void CreateRenderResource() override;
    virtual void UpdateRenderResource() override;
    virtual void ReleaseRenderResource() override;

private:
    TArray<FSkeletalMeshVertex> mVertexes;
    TArray<uint32> mIndexes;
    TArray<FStaticMeshSection> mSections;

};
