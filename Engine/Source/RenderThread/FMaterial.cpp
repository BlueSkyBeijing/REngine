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
#include "FshadingModel.h"

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

    const FShadingModel* shadingModel = FShadingModel::GetShadingModel(ShadingModel);

    if (!shadingModel)
    {
        return;
    }

    VertexShader = TSingleton<FShaderManager>::GetInstance().GetOrCreate(*shadingModel->VertexShaderInfo);

    FShaderInfo pixelShaderInfo = *shadingModel->PixelShaderInfo;
    pixelShaderInfo.Defines.insert(std::make_pair("MATERIALBLENDING_MASKED", std::to_string(int(BlendMode == BM_Masked))));
    PixelShader = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderInfo);

    FShaderInfo pixelShaderShadowInfo = *shadingModel->PixelShaderShadowInfo;
    pixelShaderShadowInfo.Defines.insert(std::make_pair("MATERIALBLENDING_MASKED", std::to_string(int(BlendMode == BM_Masked))));
    PixelShaderShadow = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderShadowInfo);

    FShaderInfo pixelShaderGPUSkinInfo = *shadingModel->PixelShaderGPUSkinInfo;
    pixelShaderGPUSkinInfo.Defines.insert(std::make_pair("MATERIALBLENDING_MASKED", std::to_string(int(BlendMode == BM_Masked))));
    PixelShaderGPUSkin = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderGPUSkinInfo);

    VertexShaderShadow = TSingleton<FShaderManager>::GetInstance().GetOrCreate(*shadingModel->VertexShaderShadowInfo);

    VertexShaderGPUSkin = TSingleton<FShaderManager>::GetInstance().GetOrCreate(*shadingModel->VertexShaderGPUSkinInfo);

    VertexShaderShadowGPUSkin = TSingleton<FShaderManager>::GetInstance().GetOrCreate(*shadingModel->VertexShaderShadowGPUSkinInfo);

    for (const auto& tex : mTexturePaths)
    {
        const FString texName = tex.first;
        const FString texFile = tex.second;
        FRHIResource* texture = TSingleton<FRHIResourceManager>::GetInstance().GetOrCreate(ERHIResourceType::RHIRT_Texture2D, texFile);
        FRHITexture* texture2D = dynamic_cast<FRHITexture*>(texture);

        mTextures.insert(std::pair<FString, FRHITexture*>(texName, texture2D));
    }

    const FString envCubeTexture = "T_EnvCube";
    const FString envMap = *FConfigManager::DefaultTexturePath + envCubeTexture + *FConfigManager::DefaultTextureFileSuffix;
    FRHIResource* texture = TSingleton<FRHIResourceManager>::GetInstance().GetOrCreate(ERHIResourceType::RHIRT_TextureCube, envMap);
    EnvMap = dynamic_cast<FRHITexture*>(texture);

    const FString preIntegratedBRDFTexture = "T_PreintegratedSkinBRDF";
    const FString preIntegratedBRDF = *FConfigManager::DefaultTexturePath + preIntegratedBRDFTexture + *FConfigManager::DefaultTextureFileSuffix;
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
