#pragma once
#include "Prerequisite.h"

#include "UObject.h"


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
