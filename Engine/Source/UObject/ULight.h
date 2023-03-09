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

public:
    virtual void Load() override;

public:
    FVector3 Direction;
    float ShadowDistance;
    float ShadowBias;

private:
};

class UPointLight : public ULight
{
public:
    UPointLight();
    virtual~UPointLight() override;

public:
    virtual void Load() override;

public:
    FVector3 Location;
    float AttenuationRadius;
    float LightFalloffExponent;

private:
};

struct FDirectionalLightData
{
    FDirectionalLightData() {}
    ~FDirectionalLightData() {}

    FVector4 Color;
    FVector3 Direction;
    float Intensity;
    float ShadowDistance;
    float ShadowBias;

};

struct FPointLightData
{
    FPointLightData() {}
    ~FPointLightData() {}

    FVector4 Color;
    FVector3 Location;
    float Intensity;
    float AttenuationRadius;
    float LightFalloffExponent;
};
