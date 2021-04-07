#pragma once
#include "Prerequisite.h"

class FRHIShaderBindings
{
    friend class FRHI;

public:
	FRHIShaderBindings();
	virtual~FRHIShaderBindings();

    void Init();
    void UnInit();

private:
};
