#pragma once
#include <d3d12.h>
#include <wrl/client.h>


class FRHIShaderBindings
{
    friend class FRHI;

public:
	FRHIShaderBindings();
	~FRHIShaderBindings();

    void Init();
    void UnInit();

private:
};
