#pragma once
#include "FRHIShader.h"

#include <string>
#include <wrl/client.h>
#include "d3d12.h"

class FD3D12Shader : public FRHIShader
{
    friend class FD3D12RHI;
public:
	FD3D12Shader();
	virtual~FD3D12Shader() override;

    void Init();
    void UnInit();

public:

private:
    Microsoft::WRL::ComPtr <ID3DBlob> mShader;

};
