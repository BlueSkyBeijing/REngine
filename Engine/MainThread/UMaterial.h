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
    float mMetallicScale;
    float mSpecularScale;
    float mRoughnessScale;
    float mOpacityScale;
    FVector4 mEmissiveColorScale;
    FVector4 mSubsurfaceColorScale;
};
