#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#include "FRHIVertex.h"
#include "FRHIBuffer.h"

#include <string>

// render thread render element

class FMaterial;
class FRHIBuffer;
class FRHIVertexBuffer;
class FRHIIndexBuffer;

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
    std::vector<std::uint16_t> Indexes;
    FRHIVertexLayout VertexLayout;
    FMaterial* Material;
    FVector3 Position;
    FVector3 Rotation;
    FVector3 Scale;
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

    std::string DebugName;

protected:

};

class FStaticMeshRenderProxy : public FRenderProxy
{
public:
    FStaticMeshRenderProxy(const FRenderProxyInitializer& initializer);
    virtual ~FStaticMeshRenderProxy() override;

    virtual void CreateRenderResource() override;
    virtual void ReleaseRenderResource() override;

private:
    FObjectConstant mObjectConstants;

    std::vector<FStaticMeshVertex> mVertexes;
    std::vector<uint16> mIndexes;
};
