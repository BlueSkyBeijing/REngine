#pragma once
#include "FRenderResource.h"

class FShader;
class FTexture2D;

class FMaterial : public FRenderResource
{
public:
	FMaterial();
	~FMaterial();

    void Init();
    void UnInit();

public:
    FShader* VertexShader;
    FShader* PixelShader;

    FTexture2D* BaseColor;
    FTexture2D* MetallicSpecularRoughness;
    FTexture2D* NormalMap;
    FTexture2D* EmissiveColor;

private:

};
