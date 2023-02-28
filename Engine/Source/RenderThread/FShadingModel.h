#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"
#include "FRHI.h"


class DLL_API FShadingModel
{
public:
    FShadingModel(int32 inValue);
    virtual ~FShadingModel();

    virtual void Init();

    virtual void UnInit();

    static const FShadingModel* GetShadingModel(int32 id);
    static void UnInitAllShadingModels();

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
    static TMap<int32, FShadingModel*> ShadingModels;
};
