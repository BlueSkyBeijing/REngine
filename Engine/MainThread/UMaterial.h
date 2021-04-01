#pragma once

#include "UObject.h"

#include <string>

class UTexture2D;
class FMaterial;

class UMaterial : UObject
{
public:
	UMaterial();
	virtual ~UMaterial() override;

public:
    std::string BaseColor;
    std::string MetallicSpecularRoughness;
    std::string EmissiveColor;
    FMaterial* Material;

public:
    virtual void Load() override;
    virtual void Unload() override;

private:
    UTexture2D* mBaseColor;
    UTexture2D* mMetallicSpecularRoughness;
    UTexture2D* mNormalMap;
    UTexture2D* mEmissiveColor;

};
