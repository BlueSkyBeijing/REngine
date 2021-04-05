#include "PrecompiledHeader.h"
#include "FRHIShader.h"
#include "FRHI.h"

#include <d3d12shader.h>
#include <d3dcompiler.h>
#include "d3dx12.h"

FRHIShader::FRHIShader() :
    FilePathName(),
    EnterPoint(),
    Target()
{
}

FRHIShader::~FRHIShader()
{
}

void FRHIShader::Init()
{
}

void FRHIShader::UnInit()
{
}
