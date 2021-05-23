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
    std::string FullResourcePath;
    std::string FullAnimSequencePath;
    std::string FullMaterialPath;

public:
    virtual void Load() override;
    virtual void Unload() override;
    void Update(float deltaSeconds);

    inline const USkeletalMesh* GetSkeletalMesh() const
    {
        return mSkeletalMesh;
    }
protected:
    virtual void createRenderProxy() override;

private:
    USkeletalMesh* mSkeletalMesh;
    UMaterial* mMaterial;
    UAnimSequence* mAnimSequence;

    FRenderProxy* mRenderProxy;
};

struct FSkeletalMeshObjectData
{
    FSkeletalMeshObjectData() {}
    ~FSkeletalMeshObjectData() {}

    FQuat Rotation;
    FVector3 Location;
    std::string ResourceName;
    std::string AnimationName;
    std::string MaterialName;
};
