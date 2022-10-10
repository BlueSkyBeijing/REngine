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

void FRenderProxy::UpdateRenderResource()
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
    BlendMode = initializer.BlendMode;
}

FStaticMeshRenderProxy::~FStaticMeshRenderProxy()
{
}

void FStaticMeshRenderProxy::CreateRenderResource()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
    VertexBuffer = rhi->CreateVertexBuffer(sizeof(FStaticMeshVertex), (uint32)mVertexes.size(), (uint8*)mVertexes.data());

    IndexBuffer = rhi->CreateIndexBuffer(sizeof(uint32), (uint32)mIndexes.size(), (uint8*)mIndexes.data());

    mObjectConstants.World.setIdentity();
    FMatrix3x3 rotation = Rotation.toRotationMatrix();
    mObjectConstants.World.block<3, 3>(0, 0) = rotation;
    mObjectConstants.World.block<1, 3>(3, 0) = Position;

    FMatrix4x4 scaleMatrix;
    scaleMatrix.setIdentity();
    scaleMatrix(0, 0) = Scale(0);
    scaleMatrix(1, 1) = Scale(1);
    scaleMatrix(2, 2) = Scale(2);
    mObjectConstants.World = scaleMatrix * mObjectConstants.World;

    mObjectConstants.Metallic = Material->Metallic;
    mObjectConstants.Specular = Material->Specular;
    mObjectConstants.Roughness = Material->Roughness;
    mObjectConstants.Opacity = Material->Opacity;

    mObjectConstants.EmissiveColor = Material->EmissiveColor;
    mObjectConstants.SubsurfaceColor = Material->SubsurfaceColor;

    ConstantBuffer = rhi->CreateConstantBuffer(sizeof(mObjectConstants), (uint8*)&mObjectConstants);

    Material->Init();

    const bool isTranslucent = Material->BlendMode == BM_Translucent;

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
    info.DepthStencilState.bEnableDepthWrite = !isTranslucent;
    info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;
    if (isTranslucent)
    {
        info.BlendState.ColorBlendOp = BO_Add;
        info.BlendState.ColorSrcBlend = BF_SourceAlpha;
        info.BlendState.ColorDestBlend = BF_InverseSourceAlpha;
        info.BlendState.AlphaBlendOp = BO_Add;
        info.BlendState.AlphaSrcBlend = BF_Zero;
        info.BlendState.AlphaDestBlend = BF_One;
    }

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    FPipelineStateInfo infoShadow;
    infoShadow.ShaderBindings = shaderBindings;
    infoShadow.VertexShader = Material->VertexShaderShadow;
    infoShadow.PixelShader = Material->PixelShaderShadow;
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
    BlendMode = initializer.BlendMode;
}

FSkeletalMeshRenderProxy::~FSkeletalMeshRenderProxy()
{
}

void FSkeletalMeshRenderProxy::CreateRenderResource()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
    VertexBuffer = rhi->CreateVertexBuffer(sizeof(FSkeletalMeshVertex), (uint32)mVertexes.size(), (uint8*)mVertexes.data());

    IndexBuffer = rhi->CreateIndexBuffer(sizeof(uint32), (uint32)mIndexes.size(), (uint8*)mIndexes.data());

    mObjectConstants.World.setIdentity();
    FMatrix3x3 rotation = Rotation.toRotationMatrix();
    mObjectConstants.World.block<3, 3>(0, 0) = rotation;
    mObjectConstants.World.block<1, 3>(3, 0) = Position;
    mObjectConstants.Metallic = Material->Metallic;
    mObjectConstants.Specular = Material->Specular;
    mObjectConstants.Roughness = Material->Roughness;
    mObjectConstants.Opacity = Material->Opacity;

    mObjectConstants.EmissiveColor = Material->EmissiveColor;
    mObjectConstants.SubsurfaceColor = Material->SubsurfaceColor;

    ConstantBuffer = rhi->CreateConstantBuffer(sizeof(mObjectConstants), (uint8*)&mObjectConstants);

    Material->Init();

    const bool isTranslucent = Material->BlendMode == BM_Translucent;

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
    info.DepthStencilState.bEnableDepthWrite = !isTranslucent;
    info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;
    if (isTranslucent)
    {
        info.BlendState.ColorBlendOp = BO_Add;
        info.BlendState.ColorSrcBlend = BF_SourceAlpha;
        info.BlendState.ColorDestBlend = BF_InverseSourceAlpha;
        info.BlendState.AlphaBlendOp = BO_Add;
        info.BlendState.AlphaSrcBlend = BF_Zero;
        info.BlendState.AlphaDestBlend = BF_One;
    }

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    FPipelineStateInfo infoGPUSkin;
    infoGPUSkin.ShaderBindings = shaderBindings;
    infoGPUSkin.VertexShader = Material->VertexShaderGPUSkin;
    infoGPUSkin.PixelShader = Material->PixelShader;
    infoGPUSkin.VertexLayout = &VertexLayout;
    infoGPUSkin.DepthStencilState.bEnableDepthWrite = true;
    infoGPUSkin.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(infoGPUSkin);

    FPipelineStateInfo infoGPUSkinShadow;
    infoGPUSkinShadow.ShaderBindings = shaderBindings;
    infoGPUSkinShadow.VertexShader = Material->VertexShaderShadowGPUSkin;
    infoGPUSkinShadow.PixelShader = Material->PixelShaderShadow;
    infoGPUSkinShadow.VertexLayout = &VertexLayout;
    infoGPUSkinShadow.DepthStencilState.bEnableDepthWrite = true;

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(infoGPUSkinShadow);

    rhi->FlushCommandQueue();
}

void FSkeletalMeshRenderProxy::UpdateRenderResource()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
    mObjectConstants.World.setIdentity();
    FMatrix3x3 rotation = Rotation.toRotationMatrix();
    mObjectConstants.World.block<3, 3>(0, 0) = rotation;
    mObjectConstants.World.block<1, 3>(3, 0) = Position;

    int32 boneIndex = 0;
    for (FMatrix4x4& BoneTransform : mObjectConstants.BoneTransforms)
    {
        if (boneIndex < BoneFinalTransforms.size())
        {
            BoneTransform = BoneFinalTransforms[boneIndex];
            boneIndex++;
        }
    }
    
    if (ConstantBuffer != nullptr)
    {
        rhi->UpdateConstantBuffer(ConstantBuffer, sizeof(mObjectConstants), (uint8*)&mObjectConstants);
    }

}

void FSkeletalMeshRenderProxy::ReleaseRenderResource()
{
    delete VertexBuffer;
    VertexBuffer = nullptr;

    delete IndexBuffer;
    IndexBuffer = nullptr;

    delete ConstantBuffer;
    ConstantBuffer = nullptr;

}
