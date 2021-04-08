#pragma once
#include "Prerequisite.h"

#include "UObject.h"


class ULight : public UObject
{
public:
    ULight();
    virtual~ULight() override;

public:
    FVector3 Color;
    float Intensity;

private:

};

class UDirectionalLight : public ULight
{
public:
    UDirectionalLight();
    virtual~UDirectionalLight() override;

public:
    FVector3 Direction;

private:
};
