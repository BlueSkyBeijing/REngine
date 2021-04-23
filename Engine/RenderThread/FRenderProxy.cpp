﻿#include "PrecompiledHeader.h"

#include "FRenderProxy.h"
#include "WindowsUtility.h"
#include "FRHI.h"
#include "FShaderManager.h"
#include "FMaterial.h"
#include "FRHIBuffer.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "FPipelineStateManager.h"
#include "FRHIPipelineState.h"
#include "MathUtility.h"


FRenderProxyInitializer::FRenderProxyInitializer()
{
}

FRenderProxyInitializer::~FRenderProxyInitializer()
{
}

FRenderProxy::FRenderProxy() :
    WorldMatrix()
{
}

FRenderProxy::~FRenderProxy()
{
}

void FRenderProxy::CreateRenderResource()
{

}

void FRenderProxy::ReleaseRenderResource()
{

}

FStaticMeshRenderProxy::FStaticMeshRenderProxy(const FRenderProxyInitializer& initializer)
{
    VertexLayout = initializer.VertexLayout;
    mVertexes = initializer.Vertexes;
    mIndexes = initializer.Indexes;
    Material = initializer.Material;
    VertexLayout = initializer.VertexLayout;
    Position = initializer.Position;
    Rotation = initializer.Rotation;
    Scale = initializer.Scale;
}

FStaticMeshRenderProxy::~FStaticMeshRenderProxy()
{
}

void FStaticMeshRenderProxy::CreateRenderResource()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
    VertexBuffer = rhi->CreateVertexBuffer(sizeof(FStaticMeshVertex), (uint32)mVertexes.size(), (uint8*)mVertexes.data());

    IndexBuffer = rhi->CreateIndexBuffer(sizeof(uint16), (uint32)mIndexes.size(), (uint8*)mIndexes.data());

    mObjectConstants.World.setIdentity();
    FMatrix3x3 rotation = Rotation.toRotationMatrix();
    mObjectConstants.World.block<3, 3>(0, 0) = rotation;
    mObjectConstants.World.block<1, 3>(3, 0) = Position;

    ConstantBuffer = rhi->CreateConstantBuffer(sizeof(mObjectConstants), (uint8*)&mObjectConstants);

    Material->Init();

    IndexCountPerInstance = static_cast<uint32>(mIndexes.size());
    InstanceCount = 1;
    StartIndexLocation = 0;
    BaseVertexLocation = 0;
    StartInstanceLocation = 0;

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(this);

    rhi->FlushCommandQueue();
}

void FStaticMeshRenderProxy::ReleaseRenderResource()
{
    delete VertexBuffer;
    VertexBuffer = nullptr;

    delete IndexBuffer;
    IndexBuffer = nullptr;

    delete ConstantBuffer;
    ConstantBuffer = nullptr;

    Material->UnInit();
    Material = nullptr;
}
