#pragma once
#include "Prerequisite.h"

class FLight
{
public:
    FLight();
    virtual ~FLight();

public:
    FVector4 Color;
    float Intensity;

private:

};

class FDirectionalLight : public FLight
{
public:
    FDirectionalLight();
    virtual ~FDirectionalLight();

public:
    FVector3 Direction;

private:

};

class FPointLight : public FLight
{
public:
    FPointLight();
    virtual ~FPointLight();

public:
    FVector3 Location;
    float AttenuationRadius;
    float LightFalloffExponent;

private:

};
