#include "PrecompiledHeader.h"

#include "UMaterial.h"
#include "UTexture.h"
#include "FMaterial.h"

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
    mBaseColor = new UTexture2D();
    mBaseColor->FullFilePathName = "Content\\Texture\\T_Default_Material_Gray_C.dds";
    mBaseColor->Load();

    //mMetallicSpecularRoughness = new UTexture2D();
    //mMetallicSpecularRoughness->FullFilePathName = "Content\\Texture\\T_Default_Material_Gray_C.dds";
    //mMetallicSpecularRoughness->Load();

    //mEmissiveColor = new UTexture2D();
    //mMetallicSpecularRoughness->FullFilePathName = "Content\\Texture\\T_Default_Material_Gray_C.dds";
    //mEmissiveColor->Load();

    Material = new FMaterial;
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
