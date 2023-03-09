#pragma once
#include "Prerequisite.h"

class FRHIShaderBindings
{
public:
    FRHIShaderBindings();
    virtual~FRHIShaderBindings();

public:
    void Init();
    void UnInit();

private:
};
