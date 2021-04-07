#pragma once
#include "Prerequisite.h"


class FRHIPipelineState
{
    friend class FRHI;

public:
	FRHIPipelineState();
	virtual ~FRHIPipelineState();

    void Init();
    void UnInit();

    FRHIShaderBindings* ShaderBindings;
    FRHIShader* VertexShader;
    FRHIShader* PixelShader;
    FRHIVertexLayout* VertexLayout;

private:

};
