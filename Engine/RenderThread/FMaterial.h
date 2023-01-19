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
    FRHITexture* GetTexture(FString name)
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

    FVector4 BaseColorScale;
    FVector4 EmissiveColorScale;
    FVector4 SubsurfaceColorScale;

    FRHITexture* EnvMap;
    FRHITexture* PreIntegratedBRDF;

    TMap<FString, FRHITexture*> mTextures;
    TMap<FString, FString> mTexturePaths;
    FString BaseColorFullFilePathName;

private:

};
