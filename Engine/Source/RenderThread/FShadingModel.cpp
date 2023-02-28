#include "PrecompiledHeader.h"

#include "FShadingModel.h"
#include "Utility.h"
#include "FConfigManager.h"

FShadingModel::FShadingModel(int32 inValue):
	Value(inValue)
{
}

FShadingModel::~FShadingModel()
{
}

TMap<int32, FShadingModel*> FShadingModel::ShadingModels;

const FShadingModel* FShadingModel::GetShadingModel(int32 id)
{
    if (ShadingModels.find(id) != ShadingModels.end())
    {
        return ShadingModels.at(id);
    }
    else if (ShadingModels.size() > 0)
    {
        return ShadingModels.begin()->second;
    }

    return nullptr;
}

void FShadingModel::Init()
{
    VertexShaderInfo.FilePathName = StringToWString(FConfigManager::EngineShaderDir + "ForwardShadingVS.hlsl");
    VertexShaderInfo.EnterPoint = "VSMain";
    VertexShaderInfo.Target = "vs_5_0";

    PixelShaderInfo.FilePathName = StringToWString(FConfigManager::EngineShaderDir + "ForwardShadingPS.hlsl");
    PixelShaderInfo.EnterPoint = "PSMain";
    PixelShaderInfo.Target = "ps_5_0";
    PixelShaderInfo.Defines.insert(std::make_pair("SHADING_MODEL", std::to_string(Value)));

    PixelShaderShadowInfo.FilePathName = StringToWString(FConfigManager::EngineShaderDir + "ShadowDepthPS.hlsl");
    PixelShaderShadowInfo.EnterPoint = "PSMain";
    PixelShaderShadowInfo.Target = "ps_5_0";

    PixelShaderGPUSkinInfo.FilePathName = StringToWString(FConfigManager::EngineShaderDir + "ForwardShadingPS.hlsl");
    PixelShaderGPUSkinInfo.EnterPoint = "PSMain";
    PixelShaderGPUSkinInfo.Target = "ps_5_0";
    PixelShaderGPUSkinInfo.Defines.insert(std::make_pair("SHADING_MODEL", std::to_string(Value)));
    PixelShaderGPUSkinInfo.Defines.insert(std::make_pair("GPU_SKIN", "1"));

    VertexShaderShadowInfo.FilePathName = StringToWString(FConfigManager::EngineShaderDir + "ShadowDepthVS.hlsl");
    VertexShaderShadowInfo.EnterPoint = "VSMain";
    VertexShaderShadowInfo.Target = "vs_5_0";

    VertexShaderGPUSkinInfo.FilePathName = StringToWString(FConfigManager::EngineShaderDir + "ForwardShadingVS.hlsl");
    VertexShaderGPUSkinInfo.EnterPoint = "VSMain";
    VertexShaderGPUSkinInfo.Target = "vs_5_0";
    VertexShaderGPUSkinInfo.Defines.insert(std::make_pair("GPU_SKIN", "1"));

    VertexShaderShadowGPUSkinInfo.FilePathName = StringToWString(FConfigManager::EngineShaderDir + "ShadowDepthVS.hlsl");
    VertexShaderShadowGPUSkinInfo.EnterPoint = "VSMain";
    VertexShaderShadowGPUSkinInfo.Target = "vs_5_0";
    VertexShaderShadowGPUSkinInfo.Defines.insert(std::make_pair("GPU_SKIN", "1"));

}

void FShadingModel::UnInit()
{
}
