#pragma once
#include "Prerequisite.h"

#include "FRHIVertex.h"
#include "FRHIBuffer.h"


// render thread render element

struct FObjectConstant
{
    FMatrix4x4 World;
    FVector4 EmissiveColor;
    FVector4 SubsurfaceColor;

    float Metallic;
    float Specular;
    float Roughness;
    float Opacity;
};

struct FSkeletonMeshConstant
{
    FMatrix4x4 World;
    FMatrix4x4 BoneTransforms[75];
    FVector4 EmissiveColor;
    FVector4 SubsurfaceColor;

    float Metallic;
    float Specular;
    float Roughness;
    float Opacity;
};

struct FStaticMeshRenderProxyInitializer
{
public:
    FStaticMeshRenderProxyInitializer();
    ~FStaticMeshRenderProxyInitializer();

    std::vector<FStaticMeshVertex> Vertexes;
    std::vector<uint32> Indexes;
    FRHIVertexLayout VertexLayout;
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;
    std::vector<FMaterial*> Materials;
    std::vector<FStaticMeshSection> mSections;
    std::vector<FMeshBatch*> MeshBatchs;

};

struct FSkeletalMeshRenderProxyInitializer
{
public:
    FSkeletalMeshRenderProxyInitializer();
    ~FSkeletalMeshRenderProxyInitializer();

    std::vector<FSkeletalMeshVertex> Vertexes;
    std::vector<uint32> Indexes;
    FRHIVertexLayout VertexLayout;
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;
    std::vector<FMaterial*> Materials;
    std::vector<FStaticMeshSection> mSections;
    std::vector<FMeshBatch*> MeshBatchs;

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
    FObjectConstant ObjectConstants;

    std::string DebugName;

};

class FSkeletalMeshBatch : public FMeshBatch
{
public:
    FSkeletalMeshBatch();
    virtual ~FSkeletalMeshBatch();

public:
    std::vector<FMatrix4x4> BoneFinalTransforms;

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
    std::vector<FMaterial*> Materials;
    FRHIVertexBuffer* VertexBuffer;
    FRHIIndexBuffer* IndexBuffer;
    FRHIVertexLayout VertexLayout;
    FVector3 Position;
    FQuat Rotation;
    FVector3 Scale;
    

    std::vector<FMeshBatch> MeshBatchs;

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
    std::vector<uint32> mIndexes;
    std::vector<FStaticMeshSection> mSections;

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

    std::vector<FMatrix4x4> BoneFinalTransforms;

private:
    FSkeletonMeshConstant mObjectConstants;

    std::vector<FSkeletalMeshVertex> mVertexes;
    std::vector<uint32> mIndexes;
    std::vector<FStaticMeshSection> mSections;

};
