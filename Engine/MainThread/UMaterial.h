#pragma once
#include "Prerequisite.h"

#include "UObject.h"


class UMaterial : public UResource
{
public:
	UMaterial();
	virtual ~UMaterial() override;

public:
    FMaterial* Material;
    FString FullFilePathName;

public:
    virtual void Load() override;
    virtual void Unload() override;

private:
    std::map<FString, FString> mTexturePaths;

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
