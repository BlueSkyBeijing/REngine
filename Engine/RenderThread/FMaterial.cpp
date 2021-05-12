#include "PrecompiledHeader.h"

#include "FMaterial.h"
#include "FRHIShader.h"
#include "FRHITexture.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRHI.h"
#include "FRenderThread.h"

FMaterial::FMaterial() :
    VertexShader(nullptr),
    PixelShader(nullptr),
    BaseColor(nullptr),
    MetallicSpecularRoughness(nullptr),
    NormalMap(nullptr),
    EmissiveColor(nullptr)
{
}

FMaterial::~FMaterial()
{
}

void FMaterial::Init()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

    const std::wstring vsFilePathName = L"Engine\\Shader\\ForwardShadingVS.hlsl";
    const std::string vsEnterPoint = "VSMain";
    const std::string vsTarget = "vs_5_0";
    std::map<std::string, std::string> defines;

    VertexShader = rhi->CreateShader(vsFilePathName, vsEnterPoint, vsTarget, defines);

    const std::wstring psFilePathName = L"Engine\\Shader\\ForwardShadingPS.hlsl";
    const std::string psEnterPoint = "PSMain";
    const std::string psTarget = "ps_5_0";

    PixelShader = rhi->CreateShader(psFilePathName, psEnterPoint, psTarget, defines);

    const std::wstring vsShadowFilePathName = L"Engine\\Shader\\ShadowDepthVS.hlsl";
    const std::string vsShadowEnterPoint = "VSMain";
    const std::string vsShadowTarget = "vs_5_0";

    VertexShaderShadow = rhi->CreateShader(vsShadowFilePathName, vsShadowEnterPoint, vsShadowTarget, defines);

    const std::wstring vsGPUSkinFilePathName = L"Engine\\Shader\\ForwardShadingVS.hlsl";
    const std::string vsGPUSkinEnterPoint = "VSMain";
    const std::string vsGPUSkinTarget = "vs_5_0";
    defines.insert(std::make_pair("GPU_SKIN", "1"));

    VertexShaderGPUSkin = rhi->CreateShader(vsGPUSkinFilePathName, vsGPUSkinEnterPoint, vsGPUSkinTarget, defines);

    const std::wstring vsGPUSkinShadowFilePathName = L"Engine\\Shader\\ShadowDepthVS.hlsl";
    const std::string vsGPUSkinShadowEnterPoint = "VSMain";
    const std::string vsGPUSkinShadowTarget = "vs_5_0";

    VertexShaderGPUSkin = rhi->CreateShader(vsGPUSkinShadowFilePathName, vsGPUSkinShadowEnterPoint, vsGPUSkinShadowTarget, defines);

    const std::wstring textureFilePathName = L"Content\\Texture\\T_Default_Material_Gray_C.dds";

    BaseColor = rhi->CreateTexture2D(textureFilePathName);

    //MetallicSpecularRoughness->Init();
    //EmissiveColor->Init();

}

void FMaterial::UnInit()
{
    delete VertexShader;
    VertexShader = nullptr;

    delete PixelShader;
    PixelShader = nullptr;

    delete VertexShaderShadow;
    VertexShader = nullptr;

    delete BaseColor;
    BaseColor = nullptr;
}
