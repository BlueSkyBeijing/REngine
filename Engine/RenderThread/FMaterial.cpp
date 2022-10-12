﻿#include "PrecompiledHeader.h"

#include "FMaterial.h"
#include "FRHIShader.h"
#include "FRHITexture.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRHI.h"
#include "FRenderThread.h"
#include "FShaderManager.h"
#include "Utility.h"
#include "FLogManager.h"
#include "FRHIResourceManager.h"

FMaterial::FMaterial() :
    VertexShader(nullptr),
    PixelShader(nullptr),
    VertexShaderGPUSkin(nullptr),
    VertexShaderShadowGPUSkin(nullptr),
    BaseColor(nullptr),
    Metallic(0.0f),
    Specular(0.5f),
    Roughness(0.5f),
    Opacity(1.0f)
{
}

FMaterial::~FMaterial()
{
}

void FMaterial::Init()
{
    FShaderInfo vertexShaderInfo;
    vertexShaderInfo.FilePathName = L"Engine\\Shader\\ForwardShadingVS.hlsl";
    vertexShaderInfo.EnterPoint = "VSMain";
    vertexShaderInfo.Target = "vs_5_0";

    VertexShader = TSingleton<FShaderManager>::GetInstance().GetOrCreate(vertexShaderInfo);

    FShaderInfo pixelShaderInfo;
    pixelShaderInfo.FilePathName = L"Engine\\Shader\\ForwardShadingPS.hlsl";
    pixelShaderInfo.EnterPoint = "PSMain";
    pixelShaderInfo.Target = "ps_5_0";
    pixelShaderInfo.Defines.insert(std::make_pair("SHADING_MODEL", std::to_string(ShadingModel)));
    pixelShaderInfo.Defines.insert(std::make_pair("MATERIALBLENDING_MASKED", std::to_string(int(BlendMode == BM_Masked))));

    PixelShader = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderInfo);

    FShaderInfo pixelShaderShadowInfo;
    pixelShaderShadowInfo.FilePathName = L"Engine\\Shader\\ShadowDepthPS.hlsl";
    pixelShaderShadowInfo.EnterPoint = "PSMain";
    pixelShaderShadowInfo.Target = "ps_5_0";
    pixelShaderShadowInfo.Defines.insert(std::make_pair("MATERIALBLENDING_MASKED", std::to_string(int(BlendMode == BM_Masked))));

    PixelShaderShadow = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderShadowInfo);


    FShaderInfo vertexShaderShadowInfo;
    vertexShaderShadowInfo.FilePathName = L"Engine\\Shader\\ShadowDepthVS.hlsl";
    vertexShaderShadowInfo.EnterPoint = "VSMain";
    vertexShaderShadowInfo.Target = "vs_5_0";

    VertexShaderShadow = TSingleton<FShaderManager>::GetInstance().GetOrCreate(vertexShaderShadowInfo);

    FShaderInfo vertexShaderGPUSkinInfo;
    vertexShaderGPUSkinInfo.FilePathName = L"Engine\\Shader\\ForwardShadingVS.hlsl";
    vertexShaderGPUSkinInfo.EnterPoint = "VSMain";
    vertexShaderGPUSkinInfo.Target = "vs_5_0";
    vertexShaderGPUSkinInfo.Defines.insert(std::make_pair("GPU_SKIN", "1"));

    VertexShaderGPUSkin = TSingleton<FShaderManager>::GetInstance().GetOrCreate(vertexShaderGPUSkinInfo);

    FShaderInfo vertexShaderShadowGPUSkinInfo;
    vertexShaderShadowGPUSkinInfo.FilePathName = L"Engine\\Shader\\ShadowDepthVS.hlsl";
    vertexShaderShadowGPUSkinInfo.EnterPoint = "VSMain";
    vertexShaderShadowGPUSkinInfo.Target = "vs_5_0";
    vertexShaderShadowGPUSkinInfo.Defines.insert(std::make_pair("GPU_SKIN", "1"));

    VertexShaderShadowGPUSkin = TSingleton<FShaderManager>::GetInstance().GetOrCreate(vertexShaderShadowGPUSkinInfo);

    FRHIResource* texture = TSingleton<FRHIResourceManager>::GetInstance().GetOrCreate(ERHIResourceType::RHIRT_Texture2D, BaseColorFullFilePathName);
    BaseColor = dynamic_cast<FRHITexture2D*>(texture);

    const std::string envCubeTexture = "T_EnvCube";
    const std::string envMap = FConfigManager::DefaultTexturePath + envCubeTexture + FConfigManager::DefaultTextureFileSuffix;
    texture = TSingleton<FRHIResourceManager>::GetInstance().GetOrCreate(ERHIResourceType::RHIRT_TextureCube, envMap);
    EnvMap = dynamic_cast<FRHITextureCube*>(texture);

    const std::string preIntegratedBRDFTexture = "T_PreintegratedSkinBRDF";
    const std::string preIntegratedBRDF = FConfigManager::DefaultTexturePath + preIntegratedBRDFTexture + FConfigManager::DefaultTextureFileSuffix;
    texture = TSingleton<FRHIResourceManager>::GetInstance().GetOrCreate(ERHIResourceType::RHIRT_Texture2D, preIntegratedBRDF);
    PreIntegratedBRDF = dynamic_cast<FRHITexture2D*>(texture);

}

void FMaterial::UnInit()
{
    VertexShader = nullptr;

    PixelShader = nullptr;

    PixelShaderShadow = nullptr;

    VertexShader = nullptr;

    VertexShaderGPUSkin = nullptr;

    VertexShaderShadowGPUSkin = nullptr;

    BaseColor = nullptr;

    EnvMap = nullptr;

    PreIntegratedBRDF = nullptr;
}
