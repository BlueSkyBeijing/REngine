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
    EBlendMode mBlendMode;
    EMaterialShadingModel mShadingModel;
    bool mTwoSided;
    float mMetallic;
    float mSpecular;
    float mRoughness;
    float mOpacity;
    FVector4 mEmissiveColor;
    FVector4 mSubsurfaceColor;
};
