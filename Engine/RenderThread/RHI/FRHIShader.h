#pragma once
#include "Prerequisite.h"

class FRHIShader
{
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
