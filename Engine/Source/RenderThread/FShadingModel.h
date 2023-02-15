#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"
#include "FRHI.h"


class FShadingModel
{
public:
    FShadingModel();
    virtual ~FShadingModel();

public:
    int32 Value;
    FShaderInfo VertexShaderInfo;
    FShaderInfo PixelShaderInfo;
    FShaderInfo PixelShaderShadowInfo;
    FShaderInfo PixelShaderGPUSkinInfo;
    FShaderInfo VertexShaderShadowInfo;
    FShaderInfo VertexShaderGPUSkinInfo;
    FShaderInfo VertexShaderShadowGPUSkinInfo;
public:
};
