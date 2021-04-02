#pragma once
#include <string>
#include <wrl/client.h>
#include "d3d12.h"

class FRHIShader
{
    friend class FRHI;
public:
	FRHIShader();
	virtual ~FRHIShader();

    void Init();
    void UnInit();

public:
    std::wstring FilePathName;
    std::string EnterPoint;
    std::string Target;

private:

};
