#include "PrecompiledHeader.h"

#include "UMaterial.h"
#include "UTexture.h"
#include "FMaterial.h"
#include "FConfigManager.h"
#include "TSingleton.h"
#include "FResourceManager.h"

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


    materialFile.read((char*)&mBlendMode, sizeof(int32));

    std::string BaseColorTextureName;
    int32 stringSize;
    materialFile.read((char*)&stringSize, sizeof(int32));
    materialFile.read((char*)BaseColorTextureName.data(), stringSize);

    std::string BaseColorTextureFullPathName = FConfigManager::DefaultTexturePath +
        std::string(BaseColorTextureName.c_str()) +
        FConfigManager::DefaultTextureFileSuffix;

    materialFile.read((char*)&mOpacity, sizeof(float));

    materialFile.close();

    mBaseColor = dynamic_cast<UTexture2D*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Texture, BaseColorTextureFullPathName));

    //mMetallicSpecularRoughness = new UTexture2D();
    //mMetallicSpecularRoughness->FullFilePathName = "Content\\Texture\\T_Default_Material_Gray_C.dds";
    //mMetallicSpecularRoughness->Load();

    //mEmissiveColor = new UTexture2D();
    //mMetallicSpecularRoughness->FullFilePathName = "Content\\Texture\\T_Default_Material_Gray_C.dds";
    //mEmissiveColor->Load();

    Material = new FMaterial;
    Material->BaseColorFullFilePathName = BaseColorTextureFullPathName;
    Material->BlendMode = mBlendMode;
    Material->Opacity = mOpacity;
}

void UMaterial::Unload()
{
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
