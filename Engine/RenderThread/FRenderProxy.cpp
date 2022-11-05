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

FMeshBatch::FMeshBatch()
{
}

FMeshBatch::~FMeshBatch()
{
}

void FMeshBatch::CreateRenderResource()
{

}

void FMeshBatch::UpdateRenderResource()
{

}

void FMeshBatch::ReleaseRenderResource()
{

}


FRenderProxy::FRenderProxy()
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
    Materials = initializer.Materials;
    Position = initializer.Position;
    Rotation = initializer.Rotation;
    Scale = initializer.Scale;
    mSections = initializer.mSections;

}

FStaticMeshRenderProxy::~FStaticMeshRenderProxy()
{
}

void FStaticMeshRenderProxy::CreateRenderResource()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
    VertexBuffer = rhi->CreateVertexBuffer(sizeof(FStaticMeshVertex), (uint32)mVertexes.size(), (uint8*)mVertexes.data());

    IndexBuffer = rhi->CreateIndexBuffer(sizeof(uint32), (uint32)mIndexes.size(), (uint8*)mIndexes.data());

    MeshBatchs.resize(mSections.size());
    for (int i = 0; i < mSections.size(); i++)
    {
        MeshBatchs[i].VertexLayout = VertexLayout;
        MeshBatchs[i].VertexBuffer = VertexBuffer;
        MeshBatchs[i].IndexBuffer = IndexBuffer;

        MeshBatchs[i].ObjectConstants.World.setIdentity();
        FMatrix3x3 rotation = Rotation.toRotationMatrix();
        MeshBatchs[i].ObjectConstants.World.block<3, 3>(0, 0) = rotation;
        MeshBatchs[i].ObjectConstants.World.block<1, 3>(3, 0) = Position;

        FMatrix4x4 scaleMatrix;
        scaleMatrix.setIdentity();
        scaleMatrix(0, 0) = Scale(0);
        scaleMatrix(1, 1) = Scale(1);
        scaleMatrix(2, 2) = Scale(2);
        MeshBatchs[i].ObjectConstants.World = scaleMatrix * MeshBatchs[i].ObjectConstants.World;

        FMaterial* material = Materials[mSections[i].MaterialIndex];
        MeshBatchs[i].ObjectConstants.Metallic = material->Metallic;
        MeshBatchs[i].ObjectConstants.Specular = material->Specular;
        MeshBatchs[i].ObjectConstants.Roughness = material->Roughness;
        MeshBatchs[i].ObjectConstants.Opacity = material->Opacity;

        MeshBatchs[i].ObjectConstants.EmissiveColor = material->EmissiveColor;
        MeshBatchs[i].ObjectConstants.SubsurfaceColor = material->SubsurfaceColor;

        MeshBatchs[i].ConstantBuffer = rhi->CreateConstantBuffer(sizeof(MeshBatchs[i].ObjectConstants), (uint8*)&MeshBatchs[i].ObjectConstants);

        material->Init();

        const bool isTranslucent = material->BlendMode == BM_Translucent;

        MeshBatchs[i].Material = material;
        MeshBatchs[i].IndexCountPerInstance = static_cast<uint32>(mIndexes.size());
        MeshBatchs[i].InstanceCount = 1;
        MeshBatchs[i].StartIndexLocation = 0;
        MeshBatchs[i].BaseVertexLocation = 0;
        MeshBatchs[i].StartInstanceLocation = 0;

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = material->VertexShader;
        info.PixelShader = material->PixelShader;
        info.VertexLayout = &MeshBatchs[i].VertexLayout;
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
        if (material->TwoSided)
        {
            info.RasterizerState.CullMode = CM_None;
        }

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

        FPipelineStateInfo infoShadow;
        infoShadow.ShaderBindings = shaderBindings;
        infoShadow.VertexShader = material->VertexShaderShadow;
        infoShadow.PixelShader = material->PixelShaderShadow;
        infoShadow.VertexLayout = &MeshBatchs[i].VertexLayout;
        infoShadow.DepthStencilState.bEnableDepthWrite = true;
        infoShadow.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;
        if (material->TwoSided)
        {
            infoShadow.RasterizerState.CullMode = CM_None;
        }

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(infoShadow);

    }

    rhi->FlushCommandQueue();
}

void FStaticMeshRenderProxy::ReleaseRenderResource()
{
    delete VertexBuffer;
    VertexBuffer = nullptr;

    delete IndexBuffer;
    IndexBuffer = nullptr;

    for (int i = 0; i < MeshBatchs.size(); i++)
    {
        delete  MeshBatchs[i].ConstantBuffer;
        MeshBatchs[i].ConstantBuffer = nullptr;
    }

}

FSkeletalMeshRenderProxy::FSkeletalMeshRenderProxy(const FSkeletalMeshRenderProxyInitializer& initializer)
{
    VertexLayout = initializer.VertexLayout;
    mVertexes = initializer.Vertexes;
    mIndexes = initializer.Indexes;
    Materials = initializer.Materials;
    VertexLayout = initializer.VertexLayout;
    Position = initializer.Position;
    Rotation = initializer.Rotation;
    Scale = initializer.Scale;
    mSections = initializer.mSections;
}

FSkeletalMeshRenderProxy::~FSkeletalMeshRenderProxy()
{
}

void FSkeletalMeshRenderProxy::CreateRenderResource()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
    VertexBuffer = rhi->CreateVertexBuffer(sizeof(FSkeletalMeshVertex), (uint32)mVertexes.size(), (uint8*)mVertexes.data());

    IndexBuffer = rhi->CreateIndexBuffer(sizeof(uint32), (uint32)mIndexes.size(), (uint8*)mIndexes.data());

    MeshBatchs.resize(mSections.size());
    for (int i = 0; i < mSections.size(); i++)
    {
        MeshBatchs[i].VertexLayout = VertexLayout;
        MeshBatchs[i].VertexBuffer = VertexBuffer;
        MeshBatchs[i].IndexBuffer = IndexBuffer;

        FMaterial* material = Materials[mSections[i].MaterialIndex];
        MeshBatchs[i].Material = material;

        MeshBatchs[i].ObjectConstants.World.setIdentity();
        FMatrix3x3 rotation = Rotation.toRotationMatrix();
        MeshBatchs[i].ObjectConstants.World.block<3, 3>(0, 0) = rotation;
        MeshBatchs[i].ObjectConstants.World.block<1, 3>(3, 0) = Position;
        MeshBatchs[i].ObjectConstants.Metallic = material->Metallic;
        MeshBatchs[i].ObjectConstants.Specular = material->Specular;
        MeshBatchs[i].ObjectConstants.Roughness = material->Roughness;
        MeshBatchs[i].ObjectConstants.Opacity = material->Opacity;

        MeshBatchs[i].ObjectConstants.EmissiveColor = material->EmissiveColor;
        MeshBatchs[i].ObjectConstants.SubsurfaceColor = material->SubsurfaceColor;

        MeshBatchs[i].ConstantBuffer = rhi->CreateConstantBuffer(sizeof(MeshBatchs[i].ObjectConstants), (uint8*)&MeshBatchs[i].ObjectConstants);

        material->Init();

        const bool isTranslucent = material->BlendMode == BM_Translucent;

        MeshBatchs[i].IndexCountPerInstance = static_cast<uint32>(mIndexes.size());
        MeshBatchs[i].InstanceCount = 1;
        MeshBatchs[i].StartIndexLocation = 0;
        MeshBatchs[i].BaseVertexLocation = 0;
        MeshBatchs[i].StartInstanceLocation = 0;

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = material->VertexShader;
        info.PixelShader = material->PixelShader;
        info.VertexLayout = &MeshBatchs[i].VertexLayout;
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
        if (material->TwoSided)
        {
            info.RasterizerState.CullMode = CM_None;
        }

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

        FPipelineStateInfo infoGPUSkin;
        infoGPUSkin.ShaderBindings = shaderBindings;
        infoGPUSkin.VertexShader = material->VertexShaderGPUSkin;
        infoGPUSkin.PixelShader = material->PixelShader;
        infoGPUSkin.VertexLayout = &MeshBatchs[i].VertexLayout;
        infoGPUSkin.DepthStencilState.bEnableDepthWrite = true;
        infoGPUSkin.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;
        if (material->TwoSided)
        {
            infoGPUSkin.RasterizerState.CullMode = CM_None;
        }

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(infoGPUSkin);

        FPipelineStateInfo infoGPUSkinShadow;
        infoGPUSkinShadow.ShaderBindings = shaderBindings;
        infoGPUSkinShadow.VertexShader = material->VertexShaderShadowGPUSkin;
        infoGPUSkinShadow.PixelShader = material->PixelShaderShadow;
        infoGPUSkinShadow.VertexLayout = &MeshBatchs[i].VertexLayout;
        infoGPUSkinShadow.DepthStencilState.bEnableDepthWrite = true;
        infoGPUSkinShadow.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;
        if (material->TwoSided)
        {
            infoGPUSkinShadow.RasterizerState.CullMode = CM_None;
        }

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(infoGPUSkinShadow);

    }

    rhi->FlushCommandQueue();
}

void FSkeletalMeshRenderProxy::UpdateRenderResource()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
    for (int i = 0; i < MeshBatchs.size(); i++)
    {
        MeshBatchs[i].ObjectConstants.World.setIdentity();
        FMatrix3x3 rotation = Rotation.toRotationMatrix();
        MeshBatchs[i].ObjectConstants.World.block<3, 3>(0, 0) = rotation;
        MeshBatchs[i].ObjectConstants.World.block<1, 3>(3, 0) = Position;

        int32 boneIndex = 0;
        for (FMatrix4x4& BoneTransform : mObjectConstants.BoneTransforms)
        {
            if (boneIndex < BoneFinalTransforms.size())
            {
                BoneTransform = BoneFinalTransforms[boneIndex];
                boneIndex++;
            }
        }

        if (MeshBatchs[i].ConstantBuffer != nullptr)
        {
            rhi->UpdateConstantBuffer(MeshBatchs[i].ConstantBuffer, sizeof(MeshBatchs[i].ObjectConstants), (uint8*)&MeshBatchs[i].ObjectConstants);
        }
    }
}

void FSkeletalMeshRenderProxy::ReleaseRenderResource()
{
    delete VertexBuffer;
    VertexBuffer = nullptr;

    delete IndexBuffer;
    IndexBuffer = nullptr;

    for (int i = 0; i < MeshBatchs.size(); i++)
    {
        delete  MeshBatchs[i].ConstantBuffer;
        MeshBatchs[i].ConstantBuffer = nullptr;
    }

}
