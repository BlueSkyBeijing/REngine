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
    materialFile.read((char*)&mMetallic, sizeof(float));
    materialFile.read((char*)&mSpecular, sizeof(float));
    materialFile.read((char*)&mRoughness, sizeof(float));
    materialFile.read((char*)&mOpacity, sizeof(float));

    materialFile.read((char*)&mEmissiveColor, sizeof(FVector4));
    materialFile.read((char*)&mSubsurfaceColor, sizeof(FVector4));

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
    Material->Metallic = mMetallic;
    Material->Specular = mSpecular;
    Material->Roughness = mRoughness;
    Material->Opacity = mOpacity;

    Material->EmissiveColor = mEmissiveColor;
    Material->SubsurfaceColor = mSubsurfaceColor;
}

void UMaterial::Unload()
{
    Material->UnInit();
    delete Material;
    Material = nullptr;
}
