#include "PrecompiledHeader.h"

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
#include "FShaderBindingsManager.h"


FStaticMeshRenderProxyInitializer::FStaticMeshRenderProxyInitializer()
{
}

FStaticMeshRenderProxyInitializer::~FStaticMeshRenderProxyInitializer()
{
}

FSkeletalMeshRenderProxyInitializer::FSkeletalMeshRenderProxyInitializer()
{
}

FSkeletalMeshRenderProxyInitializer::~FSkeletalMeshRenderProxyInitializer()
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

FStaticMeshRenderProxy::FStaticMeshRenderProxy(const FStaticMeshRenderProxyInitializer& initializer)
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

    FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
    FPipelineStateInfo info;
    info.ShaderBindings = shaderBindings;
    info.VertexShader = Material->VertexShader;
    info.PixelShader = Material->PixelShader;
    info.VertexLayout = &VertexLayout;
    info.DepthStencilState.bEnableDepthWrite = true;
    info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    FPipelineStateInfo infoShadow;
    infoShadow.ShaderBindings = shaderBindings;
    infoShadow.VertexShader = Material->VertexShaderShadow;
    infoShadow.PixelShader = nullptr;
    infoShadow.VertexLayout = &VertexLayout;
    infoShadow.DepthStencilState.bEnableDepthWrite = true;

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(infoShadow);

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

FSkeletalMeshRenderProxy::FSkeletalMeshRenderProxy(const FSkeletalMeshRenderProxyInitializer& initializer)
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

FSkeletalMeshRenderProxy::~FSkeletalMeshRenderProxy()
{
}

void FSkeletalMeshRenderProxy::CreateRenderResource()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
    VertexBuffer = rhi->CreateVertexBuffer(sizeof(FSkeletalMeshVertex), (uint32)mVertexes.size(), (uint8*)mVertexes.data());

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

    FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
    FPipelineStateInfo info;
    info.ShaderBindings = shaderBindings;
    info.VertexShader = Material->VertexShader;
    info.PixelShader = Material->PixelShader;
    info.VertexLayout = &VertexLayout;
    info.DepthStencilState.bEnableDepthWrite = true;
    info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    FPipelineStateInfo infoShadow;
    infoShadow.ShaderBindings = shaderBindings;
    infoShadow.VertexShader = Material->VertexShaderShadow;
    infoShadow.PixelShader = nullptr;
    infoShadow.VertexLayout = &VertexLayout;
    infoShadow.DepthStencilState.bEnableDepthWrite = true;

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(infoShadow);

    rhi->FlushCommandQueue();
}

void FSkeletalMeshRenderProxy::ReleaseRenderResource()
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
