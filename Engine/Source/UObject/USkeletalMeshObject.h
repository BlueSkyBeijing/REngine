#pragma once
#include "Prerequisite.h"

#include "UObject.h"

#include <string>

class USkeletalMesh;
class UMaterial;
class FRenderProxy;
class FEngine;

class USkeletalMeshObject : public UPrimitiveObject
{
public:
    USkeletalMeshObject();
    virtual ~USkeletalMeshObject() override;

public:
    FString FullResourcePath;
    FString FullAnimSequencePath;
    TArray<FString> FullMaterialPaths;

public:
    virtual void Load() override;
    virtual void Unload() override;
    void Update(float deltaSeconds);

    inline const TSharedPtr<USkeletalMesh> GetSkeletalMesh() const
    {
        return mSkeletalMesh;
    }
protected:
    virtual void createRenderProxy() override;

private:
    TSharedPtr<USkeletalMesh> mSkeletalMesh;
    TArray<TSharedPtr<UMaterial>> mMaterials;
    FAnimSequenceInstance* mAnimSequence;

    FRenderProxy* mRenderProxy;
};

struct FSkeletalMeshObjectData
{
    FSkeletalMeshObjectData() {}
    ~FSkeletalMeshObjectData() {}

    FQuat Rotation;
    FVector3 Location;
    FVector3 Scale;
    FString ResourceName;
    FString AnimationName;
    TArray<FString> MaterialNames;
};
