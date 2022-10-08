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
    mNormalMap(nullptr)
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
    materialFile.read((char*)&mMetallic, sizeof(float));
    materialFile.read((char*)&mSpecular, sizeof(float));
    materialFile.read((char*)&mRoughness, sizeof(float));
    materialFile.read((char*)&mOpacity, sizeof(float));

    materialFile.read((char*)&mEmissiveColor, sizeof(FVector4));
    materialFile.read((char*)&mSubsurfaceColor, sizeof(FVector4));

    std::string BaseColorTextureName;
    int32 stringSize;
    materialFile.read((char*)&stringSize, sizeof(int32));
    materialFile.read((char*)BaseColorTextureName.data(), stringSize);

    std::string BaseColorTextureFullPathName = FConfigManager::DefaultTexturePath +
        std::string(BaseColorTextureName.c_str()) +
        FConfigManager::DefaultTextureFileSuffix;


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
    Material->ShadingModel = mShadingModel;
    Material->Metallic = mMetallic;
    Material->Specular = mSpecular;
    Material->Roughness = mRoughness;
    Material->Opacity = mOpacity;

    Material->EmissiveColor = mEmissiveColor;
    Material->SubsurfaceColor = mSubsurfaceColor;
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

    Material->UnInit();
    delete Material;
    Material = nullptr;
}
