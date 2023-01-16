#pragma once
#include "Prerequisite.h"

#include "UObject.h"

class UWorld : UResource
{
public:
    UWorld(FEngine* engine);
    virtual ~UWorld() override;

public:
    virtual void Load() override;
    virtual void Unload() override;
    void Update(float deltaSeconds);

    inline UCamera* GetCamera() const
    {
        if (mCameras.size() > 0)
        {
            return *(mCameras.begin());
        }

        return nullptr;
    }

    virtual void LoadWorld(FString fileName);

protected:
    virtual void loadFromFile(FString fileName);

private:
    std::vector<UCamera*> mCameras;
    std::vector<UDirectionalLight*> mDirectionalLights;
    std::vector<UPointLight*> mPointLights;
    std::vector<UStaticMeshObject*> mStaticMeshObjects;
    std::vector<USkeletalMeshObject*> mSkeletalMeshObjects;
    FEngine* mEngine;
    UPlayer* mPlayer;
};
