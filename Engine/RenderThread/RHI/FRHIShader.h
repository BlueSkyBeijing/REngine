#pragma once
#include "Prerequisite.h"

class FRHIShader
{
public:
    FRHIShader();
    virtual ~FRHIShader();

public:
    void Init();
    void UnInit();

public:
    std::wstring FullFilePathName;
    FString EnterPoint;
    FString Target;

private:

};
