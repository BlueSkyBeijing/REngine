#include "PrecompiledHeader.h"

#include "FRHIResourceManager.h"
#include "FRHITexture.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRHI.h"
#include "FRenderThread.h"
#include "Utility.h"

FRHIResourceManager::FRHIResourceManager()
{
}

FRHIResourceManager::~FRHIResourceManager()
{
}

void FRHIResourceManager::Init()
{

}

void FRHIResourceManager::UnInit()
{
    for (int32 resorceType = 0; resorceType < ERHIResourceType::RHIRT_MaxNum; resorceType++)
    {
        for (auto iter = mResourceMaps[resorceType].begin(); iter != mResourceMaps[resorceType].end(); iter++)
        {
            FRHIResource* resource = iter->second;
            resource->UnInit();
            delete resource;
        }
    }

}

FRHIResource* FRHIResourceManager::GetOrCreate(ERHIResourceType resorceType, const FString& fullFileName)
{
    auto iter = mResourceMaps[resorceType].find(fullFileName);
    if (iter == mResourceMaps[resorceType].end())
    {
        switch (resorceType)
        {
        case RHIRT_Texture2D:
        {
            FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
            const std::wstring textureFilePathName = StringToWString(fullFileName);
            FRHIResource* texture = rhi->CreateTexture2D(textureFilePathName);

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, texture));

            return texture;
        }
        break;
        case RHIRT_TextureCube:
        {
            FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
            const std::wstring textureFilePathName = StringToWString(fullFileName);
            FRHIResource* texture = rhi->CreateTextureCube(textureFilePathName);

            mResourceMaps[resorceType].insert(std::make_pair(fullFileName, texture));

            return texture;
        }
        break;
        case RHIRT_MaxNum:
            break;
        default:
            break;
        }        
    }

    return iter->second;
}