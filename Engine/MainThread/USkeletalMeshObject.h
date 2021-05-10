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

public:
    virtual void Load() override;
    virtual void Unload() override;

protected:
    virtual void createRenderProxy() override;

private:

    USkeletalMesh* mSkeletalMesh;
    UMaterial* mMaterial;

    FRenderProxy* mRenderProxy;
};

struct FSkeletalMeshObjectData
{
    FSkeletalMeshObjectData() {}
    ~FSkeletalMeshObjectData() {}

    FQuat Rotation;
    FVector3 Location;
    std::string ResourceName;
};
