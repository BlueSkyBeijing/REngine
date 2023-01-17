#include "PrecompiledHeader.h"

#include "UMaterial.h"
#include "UTexture.h"
#include "FMaterial.h"
#include "FConfigManager.h"
#include "TSingleton.h"
#include "FResourceManager.h"
#include "Utility.h"

UMaterial::UMaterial()
{
}

UMaterial::~UMaterial()
{
}

void UMaterial::Load()
{
    std::ifstream materialFile(FullFilePathName, std::ios::in | std::ios::binary);
    if (!materialFile)
    {
        //print error
        return;
    }

    materialFile.read((char*)&mBlendMode, sizeof(int32));
    materialFile.read((char*)&mShadingModel, sizeof(int32));
    uint8 twoSided = 0;
    materialFile.read((char*)&twoSided, sizeof(uint8));
    mTwoSided = twoSided;
    materialFile.read((char*)&mMetallicScale, sizeof(float));
    materialFile.read((char*)&mSpecularScale, sizeof(float));
    materialFile.read((char*)&mRoughnessScale, sizeof(float));
    materialFile.read((char*)&mOpacityScale, sizeof(float));

    materialFile.read((char*)&mEmissiveColorScale, sizeof(FVector4));
    materialFile.read((char*)&mSubsurfaceColorScale, sizeof(FVector4));

    int32 numTexture;
    materialFile.read((char*)&numTexture, sizeof(int32));

    for (int32 i = 0; i < numTexture; i++)
    {
        FString texName;
        ReadUnrealString(materialFile, texName);

        FString texFile;
        ReadUnrealString(materialFile, texFile);

        texFile = FConfigManager::DefaultTexturePath +
            texFile +
            FConfigManager::DefaultTextureFileSuffix;

        mTexturePaths.insert(std::pair<FString, FString>(texName, texFile));
    }

    materialFile.close();

    Material = new FMaterial;
    Material->mTexturePaths = mTexturePaths;
    Material->BlendMode = mBlendMode;
    Material->ShadingModel = mShadingModel;
    Material->TwoSided = mTwoSided;
    Material->MetallicScale = mMetallicScale;
    Material->SpecularScale = mSpecularScale;
    Material->RoughnessScale = mRoughnessScale;
    Material->OpacityScale = mOpacityScale;

    Material->EmissiveColorScale = mEmissiveColorScale;
    Material->SubsurfaceColorScale = mSubsurfaceColorScale;
}

void UMaterial::Unload()
{
    Material->UnInit();
    delete Material;
    Material = nullptr;
}
