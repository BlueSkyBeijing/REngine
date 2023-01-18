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
    FString FullResourcePath;
    TArray<FString> FullMaterialPaths;

public:
    virtual void Load() override;
    virtual void Unload() override;

protected:
    virtual void createRenderProxy() override;

private:

    UStaticMesh* mStaticMesh;
    TArray<UMaterial*> mMaterials;

    FRenderProxy* mRenderProxy;
};

struct FStaticMeshObjectData
{
    FStaticMeshObjectData() {}
    ~FStaticMeshObjectData() {}

    FQuat Rotation;
    FVector3 Location;
    FVector3 Scale;
    FString ResourceName;
    TArray<FString> MaterialNames;
};
