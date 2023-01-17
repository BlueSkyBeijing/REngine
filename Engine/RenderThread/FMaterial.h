﻿#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"


class FMaterial : public FRHIResource
{
public:
    FMaterial();
    virtual ~FMaterial();

public:
    void Init();
    void UnInit();
    FRHITexture2D* GetTexture(FString name)
    {
        return mTextures.at(name);
    }

public:
    FRHIShader* VertexShader;
    FRHIShader* PixelShader;
    FRHIShader* PixelShaderShadow;
    FRHIShader* PixelShaderGPUSkin;

    FRHIShader* VertexShaderShadow;
    FRHIShader* VertexShaderGPUSkin;
    FRHIShader* VertexShaderShadowGPUSkin;

    EBlendMode BlendMode;
    EMaterialShadingModel ShadingModel;
    bool TwoSided;

    float MetallicScale;
    float SpecularScale;
    float RoughnessScale;
    float OpacityScale;

    FVector4 EmissiveColorScale;
    FVector4 SubsurfaceColorScale;

    FRHITextureCube* EnvMap;
    FRHITexture2D* PreIntegratedBRDF;

    std::map<FString, FRHITexture2D*> mTextures;
    std::map<FString, FString> mTexturePaths;
    FString BaseColorFullFilePathName;

private:

};
