#pragma once
#include "Prerequisite.h"


class FRHIPipelineState
{
public:
    FRHIPipelineState();
    virtual ~FRHIPipelineState();

public:
    void Init();
    void UnInit();

public:
    FRHIShaderBindings* ShaderBindings;
    FRHIShader* VertexShader;
    FRHIShader* PixelShader;
    FRHIVertexLayout* VertexLayout;

private:

};
