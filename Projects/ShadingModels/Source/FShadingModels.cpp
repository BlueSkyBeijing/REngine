#include "PrecompiledHeader.h"
#include "FShadingModels.h"
#include "Utility.h"
#include "FConfigManager.h"

class FCustomShadingModel;
FShadingModels::~FShadingModels()
{
}

void FShadingModels::Init()
{
    FShadingModel::UnInitAllShadingModels();

    FShadingModel* shadingModel = new FCustomShadingModel(0);
    shadingModel->Init();
    FShadingModel::ShadingModels->insert(std::make_pair(0, shadingModel));

    shadingModel = new FCustomShadingModel(1);
    shadingModel->Init();
    FShadingModel::ShadingModels->insert(std::make_pair(1, shadingModel));

    shadingModel = new FCustomShadingModel(2);
    shadingModel->Init();
    FShadingModel::ShadingModels->insert(std::make_pair(2, shadingModel));

    shadingModel = new FCustomShadingModel(3);
    shadingModel->Init();
    FShadingModel::ShadingModels->insert(std::make_pair(3, shadingModel));

    shadingModel = new FCustomShadingModel(4);
    shadingModel->Init();
    FShadingModel::ShadingModels->insert(std::make_pair(4, shadingModel));

    //shadingModel = new FCustomShadingModel(5);
    //shadingModel->Init();
    //FShadingModel::ShadingModels->insert(std::make_pair(5, shadingModel));

    shadingModel = new FCustomShadingModel(6);
    shadingModel->Init();
    FShadingModel::ShadingModels->insert(std::make_pair(6, shadingModel));

    //shadingModel = new FCustomShadingModel(7);
    //shadingModel->Init();
    //FShadingModel::ShadingModels->insert(std::make_pair(7, shadingModel));

    shadingModel = new FCustomShadingModel(8);
    shadingModel->Init();
    FShadingModel::ShadingModels->insert(std::make_pair(8, shadingModel));

    shadingModel = new FCustomShadingModel(9);
    shadingModel->Init();
    FShadingModel::ShadingModels->insert(std::make_pair(9, shadingModel));

    //FShadingModel::ShadingModels->insert(std::make_pair(3, new FCustomShadingModel(3)));
    //FShadingModel::ShadingModels->insert(std::make_pair(4, new FCustomShadingModel(4)));
    ////FShadingModel::ShadingModels->insert(std::make_pair(5, FCustomShadingModel(5)));
    //FShadingModel::ShadingModels->insert(std::make_pair(6, new FCustomShadingModel(6)));
    ////FShadingModel::ShadingModels->insert(std::make_pair(7, FCustomShadingModel(7)));
    //FShadingModel::ShadingModels->insert(std::make_pair(8, new FCustomShadingModel(8)));
    //FShadingModel::ShadingModels->insert(std::make_pair(9, new FCustomShadingModel(9)))


}

void FShadingModels::UnInit()
{
}

IMPLEMENT_MODULE(FShadingModels, ShadingModels)

FCustomShadingModel::FCustomShadingModel(int32 inValue):
    FShadingModel(inValue)
{

}

FCustomShadingModel::~FCustomShadingModel()
{
}

void FCustomShadingModel::Init()
{
    VertexShaderInfo = new FShaderInfo;
    PixelShaderInfo = new FShaderInfo;
    PixelShaderShadowInfo = new FShaderInfo;
    PixelShaderGPUSkinInfo = new FShaderInfo;
    VertexShaderShadowInfo = new FShaderInfo;
    VertexShaderGPUSkinInfo = new FShaderInfo;
    VertexShaderShadowGPUSkinInfo = new FShaderInfo;

    VertexShaderInfo->FilePathName = StringToWString(*FConfigManager::ProjectShaderDir + "ForwardShadingVS.hlsl");
    VertexShaderInfo->EnterPoint = "VSMain";
    VertexShaderInfo->Target = "vs_5_0";

    PixelShaderInfo->FilePathName = StringToWString(*FConfigManager::ProjectShaderDir + "ForwardShadingPS.hlsl");
    PixelShaderInfo->EnterPoint = "PSMain";
    PixelShaderInfo->Target = "ps_5_0";
    PixelShaderInfo->Defines.insert(std::make_pair("SHADING_MODEL", std::to_string(Value)));

    PixelShaderShadowInfo->FilePathName = StringToWString(*FConfigManager::ProjectShaderDir + "ShadowDepthPS.hlsl");
    PixelShaderShadowInfo->EnterPoint = "PSMain";
    PixelShaderShadowInfo->Target = "ps_5_0";

    PixelShaderGPUSkinInfo->FilePathName = StringToWString(*FConfigManager::ProjectShaderDir + "ForwardShadingPS.hlsl");
    PixelShaderGPUSkinInfo->EnterPoint = "PSMain";
    PixelShaderGPUSkinInfo->Target = "ps_5_0";
    PixelShaderGPUSkinInfo->Defines.insert(std::make_pair("SHADING_MODEL", std::to_string(Value)));
    PixelShaderGPUSkinInfo->Defines.insert(std::make_pair("GPU_SKIN", "1"));

    VertexShaderShadowInfo->FilePathName = StringToWString(*FConfigManager::ProjectShaderDir + "ShadowDepthVS.hlsl");
    VertexShaderShadowInfo->EnterPoint = "VSMain";
    VertexShaderShadowInfo->Target = "vs_5_0";

    VertexShaderGPUSkinInfo->FilePathName = StringToWString(*FConfigManager::ProjectShaderDir + "ForwardShadingVS.hlsl");
    VertexShaderGPUSkinInfo->EnterPoint = "VSMain";
    VertexShaderGPUSkinInfo->Target = "vs_5_0";
    VertexShaderGPUSkinInfo->Defines.insert(std::make_pair("GPU_SKIN", "1"));

    VertexShaderShadowGPUSkinInfo->FilePathName = StringToWString(*FConfigManager::ProjectShaderDir + "ShadowDepthVS.hlsl");
    VertexShaderShadowGPUSkinInfo->EnterPoint = "VSMain";
    VertexShaderShadowGPUSkinInfo->Target = "vs_5_0";
    VertexShaderShadowGPUSkinInfo->Defines.insert(std::make_pair("GPU_SKIN", "1"));
}

void FCustomShadingModel::UnInit()
{
    delete VertexShaderInfo;
    delete PixelShaderInfo;
    delete PixelShaderShadowInfo;
    delete PixelShaderGPUSkinInfo;
    delete VertexShaderShadowInfo;
    delete VertexShaderGPUSkinInfo;
    delete VertexShaderShadowGPUSkinInfo;

}
