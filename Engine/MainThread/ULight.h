#pragma once
#include "UObject.h"

class FEngine;

class ULight : public UObject
{
public:
	ULight();
	virtual~ULight() override;

private:

};

class UDirectionalLight : public ULight
{
public:
	UDirectionalLight(FEngine* engine);
	virtual~UDirectionalLight() override;

private:
    FEngine* mEngine;

};
