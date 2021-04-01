#include "FRenderProxy.h"
#include "FDeviceManager.h"
#include "WindowsUtility.h"
#include "DX12Utility.h"
#include "FDevice.h"
#include "FShaderManager.h"
#include "FMaterial.h"
#include "FBuffer.h"

#include <d3d12.h>
#include "d3dx12.h"

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
}

FStaticMeshRenderProxy::~FStaticMeshRenderProxy()
{
}

void FStaticMeshRenderProxy::CreateRenderResource()
{
    VertexBuffer = new FVertexBuffer;
    VertexBuffer->Vertexes = mVertexes;
    VertexBuffer->VertexLayout = VertexLayout;
    VertexBuffer->Init();

    IndexBuffer = new FIndexBuffer;
    IndexBuffer->Indexes = mIndexes;
    IndexBuffer->Init();

    ConstantBuffer = new FConstantBuffer<FObjectConstant>;

    WorldMatrix = FMatrix4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::XMMATRIX world = XMLoadFloat4x4(&WorldMatrix);

    DirectX::XMStoreFloat4x4(&mObjectConstants.World, XMMatrixTranspose(world));
    ConstantBuffer->BufferStruct = mObjectConstants;
    ConstantBuffer->Slot = 1;
    ConstantBuffer->Init();

    Material->Init();

    IndexCountPerInstance = static_cast<uint32>(IndexBuffer->Indexes.size());
    InstanceCount = 1;
    StartIndexLocation = 0;
    BaseVertexLocation = 0;
    StartInstanceLocation = 0;
}

void FStaticMeshRenderProxy::ReleaseRenderResource()
{
    VertexBuffer->UnInit();
    delete VertexBuffer;
    VertexBuffer = nullptr;

    IndexBuffer->UnInit();
    delete IndexBuffer;
    IndexBuffer = nullptr;

    ConstantBuffer->UnInit();
    delete ConstantBuffer;
    ConstantBuffer = nullptr;

    Material->UnInit();
    Material = nullptr;
}
