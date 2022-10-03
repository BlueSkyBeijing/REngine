#include "PrecompiledHeader.h"

#include "FMaterial.h"
#include "FRHIShader.h"
#include "FRHITexture.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRHI.h"
#include "FRenderThread.h"
#include "FShaderManager.h"
#include "Utility.h"

FMaterial::FMaterial() :
    VertexShader(nullptr),
    PixelShader(nullptr),
    VertexShaderGPUSkin(nullptr),
    VertexShaderShadowGPUSkin(nullptr),
    BaseColor(nullptr),
    MetallicSpecularRoughness(nullptr),
    NormalMap(nullptr),
    EmissiveColor(nullptr),
    Opacity(1.0f)
{
}

FMaterial::~FMaterial()
{
}

void FMaterial::Init()
{
    FShaderInfo vertexShaderInfo;
    vertexShaderInfo.FilePathName = L"Engine\\Shader\\ForwardShadingVS.hlsl";
    vertexShaderInfo.EnterPoint = "VSMain";
    vertexShaderInfo.Target = "vs_5_0";

    VertexShader = TSingleton<FShaderManager>::GetInstance().CreateShader(vertexShaderInfo);

    FShaderInfo pixelShaderInfo;
    pixelShaderInfo.FilePathName = L"Engine\\Shader\\ForwardShadingPS.hlsl";
    pixelShaderInfo.EnterPoint = "PSMain";
    pixelShaderInfo.Target = "ps_5_0";
    pixelShaderInfo.Defines.insert(std::make_pair("SHADING_MODEL", std::to_string(ShadingModel)));

    PixelShader = TSingleton<FShaderManager>::GetInstance().CreateShader(pixelShaderInfo);

    FShaderInfo vertexShaderShadowInfo;
    vertexShaderShadowInfo.FilePathName = L"Engine\\Shader\\ShadowDepthVS.hlsl";
    vertexShaderShadowInfo.EnterPoint = "VSMain";
    vertexShaderShadowInfo.Target = "vs_5_0";

    VertexShaderShadow = TSingleton<FShaderManager>::GetInstance().CreateShader(vertexShaderShadowInfo);

    FShaderInfo vertexShaderGPUSkinInfo;
    vertexShaderGPUSkinInfo.FilePathName = L"Engine\\Shader\\ForwardShadingVS.hlsl";
    vertexShaderGPUSkinInfo.EnterPoint = "VSMain";
    vertexShaderGPUSkinInfo.Target = "vs_5_0";
    vertexShaderGPUSkinInfo.Defines.insert(std::make_pair("GPU_SKIN", "1"));

    VertexShaderGPUSkin = TSingleton<FShaderManager>::GetInstance().CreateShader(vertexShaderGPUSkinInfo);

    FShaderInfo vertexShaderShadowGPUSkinInfo;
    vertexShaderShadowGPUSkinInfo.FilePathName = L"Engine\\Shader\\ShadowDepthVS.hlsl";
    vertexShaderShadowGPUSkinInfo.EnterPoint = "VSMain";
    vertexShaderShadowGPUSkinInfo.Target = "vs_5_0";
    vertexShaderShadowGPUSkinInfo.Defines.insert(std::make_pair("GPU_SKIN", "1"));

    VertexShaderShadowGPUSkin = TSingleton<FShaderManager>::GetInstance().CreateShader(vertexShaderShadowGPUSkinInfo);

    const std::wstring textureFilePathName = StringToWString(BaseColorFullFilePathName);

    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();
    BaseColor = rhi->CreateTexture2D(textureFilePathName);

    const std::wstring envMap = StringToWString(FConfigManager::DefaultTexturePath + "T_EnvCube" + FConfigManager::DefaultTextureFileSuffix);
    EnvMap = rhi->CreateTextureCube(envMap);

    //MetallicSpecularRoughness->Init();
    //EmissiveColor->Init();

}

void FMaterial::UnInit()
{
    VertexShader = nullptr;

    PixelShader = nullptr;

    VertexShader = nullptr;

    VertexShaderGPUSkin = nullptr;

    VertexShaderShadowGPUSkin = nullptr;

    delete BaseColor;
    BaseColor = nullptr;
}
