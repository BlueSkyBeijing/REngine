#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"


class FMaterial : public FRHIResource
{
public:
	FMaterial();
	~FMaterial();

    void Init();
    void UnInit();

public:
    FRHIShader* VertexShader;
    FRHIShader* PixelShader;

    FRHITexture2D* BaseColor;
    FRHITexture2D* MetallicSpecularRoughness;
    FRHITexture2D* NormalMap;
    FRHITexture2D* EmissiveColor;

private:

};
