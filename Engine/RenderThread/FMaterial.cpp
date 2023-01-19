#include "PrecompiledHeader.h"

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
    MetallicScale(0.0f),
    SpecularScale(0.5f),
    RoughnessScale(0.5f),
    OpacityScale(1.0f)
{
}

FMaterial::~FMaterial()
{
}

void FMaterial::Init()
{
    if (getInited())
    {
        return;
    }

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

    FShaderInfo pixelShaderGPUSkinInfo;
    pixelShaderGPUSkinInfo.FilePathName = L"Engine\\Shader\\ForwardShadingPS.hlsl";
    pixelShaderGPUSkinInfo.EnterPoint = "PSMain";
    pixelShaderGPUSkinInfo.Target = "ps_5_0";
    pixelShaderGPUSkinInfo.Defines.insert(std::make_pair("SHADING_MODEL", std::to_string(ShadingModel)));
    pixelShaderGPUSkinInfo.Defines.insert(std::make_pair("MATERIALBLENDING_MASKED", std::to_string(int(BlendMode == BM_Masked))));
    pixelShaderGPUSkinInfo.Defines.insert(std::make_pair("GPU_SKIN", "1"));

    PixelShaderGPUSkin = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderGPUSkinInfo);

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

    for (const auto& tex : mTexturePaths)
    {
        const FString texName = tex.first;
        const FString texFile = tex.second;
        FRHIResource* texture = TSingleton<FRHIResourceManager>::GetInstance().GetOrCreate(ERHIResourceType::RHIRT_Texture2D, texFile);
        FRHITexture* texture2D = dynamic_cast<FRHITexture*>(texture);

        mTextures.insert(std::pair<FString, FRHITexture*>(texName, texture2D));
    }

    const FString envCubeTexture = "T_EnvCube";
    const FString envMap = FConfigManager::DefaultTexturePath + envCubeTexture + FConfigManager::DefaultTextureFileSuffix;
    FRHIResource* texture = TSingleton<FRHIResourceManager>::GetInstance().GetOrCreate(ERHIResourceType::RHIRT_TextureCube, envMap);
    EnvMap = dynamic_cast<FRHITexture*>(texture);

    const FString preIntegratedBRDFTexture = "T_PreintegratedSkinBRDF";
    const FString preIntegratedBRDF = FConfigManager::DefaultTexturePath + preIntegratedBRDFTexture + FConfigManager::DefaultTextureFileSuffix;
    texture = TSingleton<FRHIResourceManager>::GetInstance().GetOrCreate(ERHIResourceType::RHIRT_Texture2D, preIntegratedBRDF);
    PreIntegratedBRDF = dynamic_cast<FRHITexture*>(texture);

    setInited(true);
}

void FMaterial::UnInit()
{
    VertexShader = nullptr;

    PixelShader = nullptr;

    PixelShaderShadow = nullptr;

    VertexShader = nullptr;

    VertexShaderGPUSkin = nullptr;

    VertexShaderShadowGPUSkin = nullptr;

    EnvMap = nullptr;

    PreIntegratedBRDF = nullptr;

    mTextures.clear();

    mTexturePaths.clear();

    setInited(false);

}
