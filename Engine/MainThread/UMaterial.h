#pragma once
#include "Prerequisite.h"

#include "UObject.h"


class UMaterial : public UResource
{
public:
	UMaterial();
	virtual ~UMaterial() override;

public:
    std::string BaseColor;
    std::string MetallicSpecularRoughness;
    std::string EmissiveColor;
    FMaterial* Material;
    std::string FullFilePathName;

public:
    virtual void Load() override;
    virtual void Unload() override;

private:
    UTexture2D* mBaseColor;
    UTexture2D* mMetallicSpecularRoughness;
    UTexture2D* mNormalMap;
    UTexture2D* mEmissiveColor;
    EBlendMode mBlendMode;
    EMaterialShadingModel mShadingModel;
    float mMetallic;
    float mSpecular;
    float mRoughness;
    float mOpacity;
};
