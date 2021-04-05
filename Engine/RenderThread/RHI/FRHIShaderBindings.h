#pragma once

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
