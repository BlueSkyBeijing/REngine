#pragma once
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
    TSharedPtr<FRHIShader> VertexShader;
    TSharedPtr<FRHIShader>PixelShader;
    TSharedPtr<FRHIShader> PixelShaderShadow;
    TSharedPtr<FRHIShader> PixelShaderGPUSkin;

    TSharedPtr<FRHIShader> VertexShaderShadow;
    TSharedPtr<FRHIShader> VertexShaderGPUSkin;
    TSharedPtr<FRHIShader> VertexShaderShadowGPUSkin;

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
