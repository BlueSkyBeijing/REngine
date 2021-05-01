#pragma once
#include "Prerequisite.h"


class FRHIResource
{
public:
    FRHIResource();
    virtual~FRHIResource();

    virtual void Init() {}
    virtual void UnInit() {}

public:
    int32 PosInHeap;

private:

};

class FRHISamplerState
{
public:
};

class FRHIRasterizerState
{
public:
};

class FRHIDepthStencilState
{
public:
};

class FRHIBlendState
{
public:
};
