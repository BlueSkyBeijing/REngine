#include "FMaterial.h"
#include "FRHIShader.h"
#include "FRHITexture.h"

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
    VertexShader = new FRHIShader;
    VertexShader->FilePathName = L"Engine\\Shader\\ForwardShadingVS.hlsl";
    VertexShader->EnterPoint = "VSMain";
    VertexShader->Target = "vs_5_0";
    VertexShader->Init();

    PixelShader = new FRHIShader;
    PixelShader->FilePathName = L"Engine\\Shader\\ForwardShadingPS.hlsl";
    PixelShader->EnterPoint = "PSMain";
    PixelShader->Target = "ps_5_0";
    PixelShader->Init();

    BaseColor = new FRHITexture2D;
    BaseColor->FilePathName = L"Content\\Texture\\T_Stone_C.dds";
    BaseColor->Slot = 0;
    BaseColor->Init();

    //MetallicSpecularRoughness->Init();
    //EmissiveColor->Init();

}

void FMaterial::UnInit()
{
    VertexShader->UnInit();
    delete VertexShader;
    VertexShader = nullptr;

    PixelShader->UnInit();
    delete PixelShader;
    PixelShader = nullptr;

    BaseColor->UnInit();
    delete BaseColor;
    BaseColor = nullptr;
}
