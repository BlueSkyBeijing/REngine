#pragma once
#include "Prerequisite.h"

#include "UObject.h"


class ULight : public UObject
{
public:
    ULight();
    virtual~ULight() override;

public:
    FVector4 Color;
    float Intensity;

private:

};

class UDirectionalLight : public ULight
{
public:
    UDirectionalLight();
    virtual~UDirectionalLight() override;

    virtual void Load() override;

public:
    FVector3 Direction;
    float Intensity;

private:
};


struct FDirectionalLightData
{
    FDirectionalLightData() {}
    ~FDirectionalLightData() {}

    FVector4 Color;
    FVector3 Direction;
    float Intensity;
};
