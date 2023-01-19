#include "PrecompiledHeader.h"

#include "FResourceManager.h"
#include "UStaticMesh.h"
#include "TSingleton.h"
#include "UStaticMesh.h"
#include "USkeletalMesh.h"
#include "UMaterial.h"
#include "UTexture.h"

FResourceManager::FResourceManager()
{
}

FResourceManager::~FResourceManager()
{
}

void FResourceManager::Init()
{

}

void FResourceManager::UnInit()
{
    for (int32 resorceType = 0; resorceType < EResourceType::RT_MaxNum; resorceType++)
    {
        for (auto iter = mResourceMaps[resorceType].begin(); iter != mResourceMaps[resorceType].end(); iter++)
        {
            UResource* resource = iter->second;
            resource->Unload();
            delete resource;
        }
    }

}

UResource* FResourceManager::GetOrCreate(EResourceType resorceType, const FString& fullFileName)
{
    auto iter = mResourceMaps[resorceType].find(fullFileName);
    if (iter == mResourceMaps[resorceType].end())
    {
        switch (resorceType)
        {
        case RT_StaticMesh:
        {
            UStaticMesh* staticMesh = new UStaticMesh();
            staticMesh->FullFilePathName = fullFileName;
            staticMesh->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, staticMesh));

            return staticMesh;
        }
            break;
        case RT_SkeletalMesh:
        {
            USkeletalMesh* skeletalMesh = new USkeletalMesh();
            skeletalMesh->FullFilePathName = fullFileName;
            skeletalMesh->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, skeletalMesh));

            return skeletalMesh;
        }

            break;
        case RT_Skeleton:
        {
            USkeleton* skeleton = new USkeleton();
            skeleton->FullFilePathName = fullFileName;
            skeleton->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, skeleton));

            return skeleton;
        }
        break;
        case RT_Animation:
        {
            UAnimSequence* animSequence = new UAnimSequence();
            animSequence->FullFilePathName = fullFileName;
            //animSequence->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, animSequence));

            return animSequence;
        }
            break;
        case RT_Texture:
        {
            UTexture2D* texture2D = new UTexture2D();
            texture2D->FullFilePathName = fullFileName;
            texture2D->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, texture2D));

            return texture2D;
        }
        break;
        case RT_Material:
        {
            UMaterial* material = new UMaterial();
            material->FullFilePathName = fullFileName;
            material->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, material));

            return material;
        }
            break;
        case RT_Map:
        {
        }
            break;
        case RT_MaxNum:
            break;
        default:
            break;
        }        
    }

    return iter->second;
}