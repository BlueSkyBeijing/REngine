#pragma once

#include "UObject.h"

#include <string>

class UStaticMesh;
class UMaterial;
class FRenderProxy;
class FEngine;

class UStaticMeshObject : public UPrimitiveObject
{
public:
	UStaticMeshObject(FEngine* engine);
	virtual ~UStaticMeshObject() override;

public:
	std::string StaticMeshPath;

public:
    virtual void Load() override;
    virtual void Unload() override;

protected:
	virtual void createRenderProxy() override;

private:

	UStaticMesh* mStaticMesh;
	UMaterial* mMaterial;
    FEngine* mEngine;

	FRenderProxy* mRenderProxy;
};

