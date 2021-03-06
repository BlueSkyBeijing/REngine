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
        return *(mCameras.begin());
    }

private:
    std::vector<UCamera*> mCameras;
    std::vector<UDirectionalLight*> mDirectionalLights;
    std::vector<UPointLight*> mPointLights;
    std::vector<UStaticMeshObject*> mStaticMeshObjects;
    std::vector<USkeletalMeshObject*> mSkeletalMeshObjects;
    FEngine* mEngine;
    UPlayer* mPlayer;
};
