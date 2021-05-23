#include "PrecompiledHeader.h"

#include "UMaterial.h"
#include "UTexture.h"
#include "FMaterial.h"
#include "FConfigManager.h"

UMaterial::UMaterial() :
    mBaseColor(nullptr),
    mMetallicSpecularRoughness(nullptr),
    mNormalMap(nullptr),
    mEmissiveColor(nullptr)
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

    std::string BaseColorTextureName;
    int32 stringSize;
    materialFile.read((char*)&stringSize, sizeof(int32));
    materialFile.read((char*)BaseColorTextureName.data(), stringSize);

    std::string BaseColorTextureFullPathName = FConfigManager::DefaultTexturePath +
        std::string(BaseColorTextureName.c_str()) +
        FConfigManager::DefaultTextureFileSuffix;

    materialFile.close();

    mBaseColor = new UTexture2D();
    mBaseColor->FullFilePathName = BaseColorTextureFullPathName;
    mBaseColor->Load();

    //mMetallicSpecularRoughness = new UTexture2D();
    //mMetallicSpecularRoughness->FullFilePathName = "Content\\Texture\\T_Default_Material_Gray_C.dds";
    //mMetallicSpecularRoughness->Load();

    //mEmissiveColor = new UTexture2D();
    //mMetallicSpecularRoughness->FullFilePathName = "Content\\Texture\\T_Default_Material_Gray_C.dds";
    //mEmissiveColor->Load();

    Material = new FMaterial;
    Material->BaseColorFullFilePathName = BaseColorTextureFullPathName;
}

void UMaterial::Unload()
{
    mBaseColor->Unload();
    delete mBaseColor;
    mBaseColor = nullptr;

    //mMetallicSpecularRoughness->Unload();
    //delete mMetallicSpecularRoughness;
    //mMetallicSpecularRoughness = nullptr;

    //mEmissiveColor->Unload();
    //delete mEmissiveColor;
    //mEmissiveColor = nullptr;

    delete Material;
    Material = nullptr;
}
