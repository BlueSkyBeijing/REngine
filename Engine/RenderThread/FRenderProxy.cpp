#include "FRenderProxy.h"
#include "WindowsUtility.h"
#include "FRHI.h"
#include "FShaderManager.h"
#include "FMaterial.h"
#include "FRHIBuffer.h"
#include "FEngine.h"
#include "FRenderThread.h"

FRenderProxyInitializer::FRenderProxyInitializer()
{
}

FRenderProxyInitializer::~FRenderProxyInitializer()
{
}

FRenderProxy::FRenderProxy():
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

    IndexBuffer = rhi->CreateIndexBuffer(sizeof(uint16), (uint32)mIndexes.size(), (uint8*) mIndexes.data());

    WorldMatrix = FMatrix4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::XMMATRIX world = XMLoadFloat4x4(&WorldMatrix);

    DirectX::XMStoreFloat4x4(&mObjectConstants.World, XMMatrixTranspose(world));

    ConstantBuffer = rhi->CreateConstantBuffer(sizeof(mObjectConstants), (uint8*)&mObjectConstants, 1);

    Material->Init();

    IndexCountPerInstance = static_cast<uint32>(mIndexes.size());
    InstanceCount = 1;
    StartIndexLocation = 0;
    BaseVertexLocation = 0;
    StartInstanceLocation = 0;
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
