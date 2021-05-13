#pragma once
#include "Prerequisite.h"

#include "UObject.h"

class UStaticMeshObject;
class UCamera;
class UDirectionalLight;
class FEngine;

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
    std::vector<UStaticMeshObject*> mStaticMeshObjects;
    std::vector<USkeletalMeshObject*> mSkeletalMeshObjects;
    FEngine* mEngine;
};
