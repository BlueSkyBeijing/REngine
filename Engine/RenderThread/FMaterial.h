#pragma once
#include "FRHIResource.h"

class FRHIShader;
class FRHITexture2D;

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
