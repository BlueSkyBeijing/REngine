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
    delete  ConstantBuffer;
    ConstantBuffer = nullptr;
}

FStaticMeshBatch::FStaticMeshBatch()
{
}

FStaticMeshBatch::~FStaticMeshBatch()
{
}

FSkeletalMeshBatch::FSkeletalMeshBatch()
{
}

FSkeletalMeshBatch::~FSkeletalMeshBatch()
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
        FStaticMeshBatch* meshBatch = new FStaticMeshBatch();
        MeshBatchs[i] = meshBatch;
        meshBatch->VertexLayout = VertexLayout;
        meshBatch->VertexBuffer = VertexBuffer;
        meshBatch->IndexBuffer = IndexBuffer;

        meshBatch->ObjectConstants.World.setIdentity();
        FMatrix3x3 rotation = Rotation.toRotationMatrix();
        meshBatch->ObjectConstants.World.block<3, 3>(0, 0) = rotation;
        meshBatch->ObjectConstants.World.block<1, 3>(3, 0) = Position;

        FMatrix4x4 scaleMatrix;
        scaleMatrix.setIdentity();
        scaleMatrix(0, 0) = Scale(0);
        scaleMatrix(1, 1) = Scale(1);
        scaleMatrix(2, 2) = Scale(2);
        meshBatch->ObjectConstants.World = scaleMatrix * meshBatch->ObjectConstants.World;

        FMaterial* material = Materials[mSections[i].MaterialIndex];
        meshBatch->ObjectConstants.Metallic = material->Metallic;
        meshBatch->ObjectConstants.Specular = material->Specular;
        meshBatch->ObjectConstants.Roughness = material->Roughness;
        meshBatch->ObjectConstants.Opacity = material->Opacity;

        meshBatch->ObjectConstants.EmissiveColor = material->EmissiveColor;
        meshBatch->ObjectConstants.SubsurfaceColor = material->SubsurfaceColor;

        meshBatch->ConstantBuffer = rhi->CreateConstantBuffer(sizeof(meshBatch->ObjectConstants), (uint8*)&meshBatch->ObjectConstants);

        material->Init();

        meshBatch->DebugName = DebugName;

        const bool isTranslucent = material->BlendMode == BM_Translucent;

        meshBatch->Material = material;
        meshBatch->IndexCountPerInstance = mSections[i].NumTriangles * 3;
        meshBatch->InstanceCount = 1;
        meshBatch->StartIndexLocation = mSections[i].FirstIndex;
        meshBatch->BaseVertexLocation = 0;
        meshBatch->StartInstanceLocation = 0;

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = material->VertexShader;
        info.PixelShader = material->PixelShader;
        info.VertexLayout = &meshBatch->VertexLayout;
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
        infoShadow.VertexLayout = &meshBatch->VertexLayout;
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

    MeshBatchs.clear();
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
        FSkeletalMeshBatch* meshBatch = new FSkeletalMeshBatch();
        MeshBatchs[i] = meshBatch;

        meshBatch->VertexLayout = VertexLayout;
        meshBatch->VertexBuffer = VertexBuffer;
        meshBatch->IndexBuffer = IndexBuffer;

        FMaterial* material = Materials[mSections[i].MaterialIndex];
        meshBatch->Material = material;
        meshBatch->DebugName = DebugName;

        meshBatch->ObjectConstants.World.setIdentity();
        FMatrix3x3 rotation = Rotation.toRotationMatrix();
        meshBatch->ObjectConstants.World.block<3, 3>(0, 0) = rotation;
        meshBatch->ObjectConstants.World.block<1, 3>(3, 0) = Position;
        meshBatch->ObjectConstants.Metallic = material->Metallic;
        meshBatch->ObjectConstants.Specular = material->Specular;
        meshBatch->ObjectConstants.Roughness = material->Roughness;
        meshBatch->ObjectConstants.Opacity = material->Opacity;

        meshBatch->ObjectConstants.EmissiveColor = material->EmissiveColor;
        meshBatch->ObjectConstants.SubsurfaceColor = material->SubsurfaceColor;

        meshBatch->ConstantBuffer = rhi->CreateConstantBuffer(sizeof(meshBatch->ObjectConstants), (uint8*)&meshBatch->ObjectConstants);

        material->Init();

        const bool isTranslucent = material->BlendMode == BM_Translucent;

        meshBatch->IndexCountPerInstance = mSections[i].NumTriangles * 3;
        meshBatch->InstanceCount = 1;
        meshBatch->StartIndexLocation = mSections[i].FirstIndex;
        meshBatch->BaseVertexLocation = 0;
        meshBatch->StartInstanceLocation = 0;

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = material->VertexShader;
        info.PixelShader = material->PixelShader;
        info.VertexLayout = &meshBatch->VertexLayout;
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
        infoGPUSkin.VertexLayout = &meshBatch->VertexLayout;
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
        infoGPUSkinShadow.VertexLayout = &meshBatch->VertexLayout;
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
        FSkeletalMeshBatch* meshBatch = dynamic_cast<FSkeletalMeshBatch*>(MeshBatchs[i]);

        meshBatch->ObjectConstants.World.setIdentity();
        FMatrix3x3 rotation = Rotation.toRotationMatrix();
        meshBatch->ObjectConstants.World.block<3, 3>(0, 0) = rotation;
        meshBatch->ObjectConstants.World.block<1, 3>(3, 0) = Position;

        int32 boneIndex = 0;
        for (FMatrix4x4& BoneTransform : meshBatch->ObjectConstants.BoneTransforms)
        {
            if (boneIndex < meshBatch->BoneFinalTransforms.size())
            {
                BoneTransform = meshBatch->BoneFinalTransforms[boneIndex];
                boneIndex++;
            }
        }

        if (meshBatch->ConstantBuffer != nullptr)
        {
            rhi->UpdateConstantBuffer(meshBatch->ConstantBuffer, sizeof(meshBatch->ObjectConstants), (uint8*)&meshBatch->ObjectConstants);
        }
    }
}

void FSkeletalMeshRenderProxy::ReleaseRenderResource()
{
    delete VertexBuffer;
    VertexBuffer = nullptr;

    delete IndexBuffer;
    IndexBuffer = nullptr;

    MeshBatchs.clear();
}
