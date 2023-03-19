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

public:
    virtual void Load() override;
    virtual void Unload() override;

private:
    TMap<FString, FString> mTexturePaths;

    EBlendMode mBlendMode;
    EMaterialShadingModel mShadingModel;
    bool mTwoSided;
    float mMetallicScale;
    float mSpecularScale;
    float mRoughnessScale;
    float mOpacityScale;
    FVector4 mBaseColorScale;
    FVector4 mEmissiveColorScale;
    FVector4 mSubsurfaceColorScale;
};