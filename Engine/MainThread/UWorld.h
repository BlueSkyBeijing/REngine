#pragma once
#include "UObject.h"

#include <vector>
#include <string>

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

private:
	std::vector<UStaticMeshObject*> mStaticMeshObjects;
	UCamera* mCamera;
	UDirectionalLight* mDirectionalLight;
    FEngine* mEngine;
};
