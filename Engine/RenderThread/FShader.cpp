#include "FShader.h"
#include "FDeviceManager.h"
#include "FDevice.h"

#include <d3d12shader.h>
#include <d3dcompiler.h>
#include "d3dx12.h"

FShader::FShader() :
    FilePathName(),
    EnterPoint(),
    Target()
{
}

FShader::~FShader()
{
}

void FShader::Init()
{
    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->CreateShader(this);
}

void FShader::UnInit()
{
}
