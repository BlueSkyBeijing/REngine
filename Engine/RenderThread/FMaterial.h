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

    FRHIShader* VertexShaderShadow;

    FRHITexture2D* BaseColor;
    FRHITexture2D* MetallicSpecularRoughness;
    FRHITexture2D* NormalMap;
    FRHITexture2D* EmissiveColor;

private:

};
