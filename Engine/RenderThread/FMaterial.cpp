#include "FMaterial.h"
#include "FRHIShader.h"
#include "FRHITexture.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRHI.h"
#include "FRenderThread.h"

FMaterial::FMaterial():
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

    VertexShader = rhi->CreateShader(vsFilePathName, vsEnterPoint, vsTarget);

    const std::wstring psFilePathName = L"Engine\\Shader\\ForwardShadingPS.hlsl";
    const std::string psEnterPoint = "PSMain";
    const std::string psTarget = "ps_5_0";

    PixelShader = rhi->CreateShader(psFilePathName, psEnterPoint, psTarget);

    const std::wstring textureFilePathName = L"Content\\Texture\\T_Stone_C.dds";
    const int32 textureSlot = 0;

    BaseColor = rhi->CreateTexture2D(textureFilePathName, textureSlot);

    //MetallicSpecularRoughness->Init();
    //EmissiveColor->Init();

}

void FMaterial::UnInit()
{
    delete VertexShader;
    VertexShader = nullptr;

    delete PixelShader;
    PixelShader = nullptr;

    delete BaseColor;
    BaseColor = nullptr;
}
