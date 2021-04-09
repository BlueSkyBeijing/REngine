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

    inline UCamera* GetCamera() const
    {
        return mCamera;
    }

private:
    std::vector<UStaticMeshObject*> mStaticMeshObjects;
    UCamera* mCamera;
    UDirectionalLight* mDirectionalLight;
    FEngine* mEngine;
};
