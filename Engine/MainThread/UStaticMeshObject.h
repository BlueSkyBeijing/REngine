#pragma once
#include "Prerequisite.h"

#include "UObject.h"

#include <string>

class UStaticMesh;
class UMaterial;
class FRenderProxy;
class FEngine;

class UStaticMeshObject : public UPrimitiveObject
{
public:
    UStaticMeshObject();
    virtual ~UStaticMeshObject() override;

public:
    std::string FullResourcePath;
    std::string FullMaterialPath;

public:
    virtual void Load() override;
    virtual void Unload() override;

protected:
    virtual void createRenderProxy() override;

private:

    UStaticMesh* mStaticMesh;
    UMaterial* mMaterial;

    FRenderProxy* mRenderProxy;
};

struct FStaticMeshObjectData
{
    FStaticMeshObjectData() {}
    ~FStaticMeshObjectData() {}

    FQuat Rotation;
    FVector3 Location;
    FVector3 Scale;
    std::string ResourceName;
    std::string MaterialName;
};
