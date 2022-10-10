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

public:
    FRHIShader* VertexShader;
    FRHIShader* PixelShader;
    FRHIShader* PixelShaderShadow;

    FRHIShader* VertexShaderShadow;
    FRHIShader* VertexShaderGPUSkin;
    FRHIShader* VertexShaderShadowGPUSkin;

    FRHITexture2D* BaseColor;

    EBlendMode BlendMode;
    EMaterialShadingModel ShadingModel;
    float Metallic;
    float Specular;
    float Roughness;
    float Opacity;

    FVector4 EmissiveColor;
    FVector4 SubsurfaceColor;

    FRHITextureCube* EnvMap;
    FRHITexture2D* PreIntegratedBRDF;

    std::string BaseColorFullFilePathName;

private:

};
