#pragma once
#include "Prerequisite.h"

class FShader
{
public:
    FShader();
    virtual ~FShader();

public:
    void Init();
    void UnInit();
};

class FGlobalShader : public FShader
{
public:
    FGlobalShader();
    virtual ~FGlobalShader();

};

class FMaterialShader : public FShader
{
public:
    FMaterialShader();
    virtual ~FMaterialShader();

};

class FShaderType
{
public:
    FShaderType();
    virtual ~FShaderType();

};