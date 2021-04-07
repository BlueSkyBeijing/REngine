#pragma once
#include "Prerequisite.h"


class FRHIResource
{
public:
	FRHIResource();
    virtual~FRHIResource();

    virtual void Init() {}
    virtual void UnInit() {}

private:

};
