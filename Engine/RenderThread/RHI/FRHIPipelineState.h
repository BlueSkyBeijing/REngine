#pragma once
#include <d3d12.h>
#include <wrl/client.h>

class FRHIShaderBindings;
class FRHIShader;
class FRHIVertexLayout;

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
