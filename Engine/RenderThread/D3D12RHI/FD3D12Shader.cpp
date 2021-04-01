#include "FD3D12Shader.h"
#include "FD3D12RHI.h"

#include <d3d12shader.h>
#include <d3dcompiler.h>
#include "d3dx12.h"

FD3D12Shader::FD3D12Shader() :
    FilePathName(),
    EnterPoint(),
    Target()
{
}

FD3D12Shader::~FD3D12Shader()
{
}

void FD3D12Shader::Init()
{
    TSingleton<FD3D12RHIManager>::GetInstance().GetRootDevice()->CreateShader(this);
}

void FD3D12Shader::UnInit()
{
}
