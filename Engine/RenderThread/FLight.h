#pragma once
#include "Prerequisite.h"

class FLight
{
public:
    FLight();
    ~FLight();

public:
    FVector4 Color;
    float Intensity;

private:

};

class FDirectionalLight : public FLight
{
public:
    FDirectionalLight();
    ~FDirectionalLight();

public:
    FVector3 Direction;
    float Intensity;

private:

};
