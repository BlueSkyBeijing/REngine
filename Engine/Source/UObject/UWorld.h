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

    inline TSharedPtr<UCamera> GetCamera() const
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
    TArray<TSharedPtr<UCamera>> mCameras;
    TArray<TSharedPtr<UDirectionalLight>> mDirectionalLights;
    TArray<TSharedPtr<UPointLight>> mPointLights;
    TArray<TSharedPtr<UStaticMeshObject>> mStaticMeshObjects;
    TArray<TSharedPtr<USkeletalMeshObject>> mSkeletalMeshObjects;
    FEngine* mEngine;
    TSharedPtr<UPlayer> mPlayer;
};
