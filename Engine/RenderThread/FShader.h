#pragma once
#include <string>
#include <wrl/client.h>
#include "d3d12.h"

class FShader
{
    friend class FDevice;
public:
	FShader();
	~FShader();

    void Init();
    void UnInit();

public:
    std::wstring FilePathName;
    std::string EnterPoint;
    std::string Target;

private:
    Microsoft::WRL::ComPtr <ID3DBlob> mShader;

};
