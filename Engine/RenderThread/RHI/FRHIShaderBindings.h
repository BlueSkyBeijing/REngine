#pragma once
#include "Prerequisite.h"

class FRHIShaderBindings
{
public:
    FRHIShaderBindings();
    virtual~FRHIShaderBindings();

    void Init();
    void UnInit();

private:
};
