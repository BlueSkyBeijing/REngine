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
            TSharedPtr<UResource> resource = iter->second;
            resource->Unload();
        }
    }

    mResourceMaps->clear();
}

TSharedPtr<UResource> FResourceManager::GetOrCreate(EResourceType resorceType, const FString& fullFileName)
{
    auto iter = mResourceMaps[resorceType].find(fullFileName);
    if (iter == mResourceMaps[resorceType].end())
    {
        switch (resorceType)
        {
        case RT_StaticMesh:
        {
            TSharedPtr<UResource> staticMesh = TMakeShared<UStaticMesh>();
            staticMesh->FullFilePathName = fullFileName;
            staticMesh->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, staticMesh));

            return staticMesh;
        }
            break;
        case RT_SkeletalMesh:
        {
            TSharedPtr<UResource> skeletalMesh = TMakeShared<USkeletalMesh>();
            skeletalMesh->FullFilePathName = fullFileName;
            skeletalMesh->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, skeletalMesh));

            return skeletalMesh;
        }

            break;
        case RT_Skeleton:
        {
            TSharedPtr<UResource> skeleton(new USkeleton());
            skeleton->FullFilePathName = fullFileName;
            skeleton->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, skeleton));

            return skeleton;
        }
        break;
        case RT_Animation:
        {
            TSharedPtr<UResource> animSequence = TMakeShared<UAnimSequence>();
            animSequence->FullFilePathName = fullFileName;
            //animSequence->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, animSequence));

            return animSequence;
        }
            break;
        case RT_Texture:
        {
            TSharedPtr<UResource> texture2D = TMakeShared<UTexture2D>();
            texture2D->FullFilePathName = fullFileName;
            texture2D->Load();

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, texture2D));

            return texture2D;
        }
        break;
        case RT_Material:
        {
            TSharedPtr<UResource> material = TMakeShared<UMaterial>();
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