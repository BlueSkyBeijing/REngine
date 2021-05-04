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
    float Intensity;

private:

};
