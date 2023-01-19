#include "PrecompiledHeader.h"

#include "FPostProcessing.h"
#include "FRHIRenderTarget.h"
#include "FRenderProxy.h"
#include "FScene.h"
#include "FRHI.h"
#include "FPipelineStateManager.h"
#include "FShaderBindingsManager.h"
#include "FRHIShader.h"
#include "FRHIPipelineState.h"
#include "FRHIShaderBindings.h"
#include "TSingleton.h"
#include "FRHIBuffer.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "FRHIVertex.h"
#include "FRHIBuffer.h"
#include "FShaderManager.h"
#include "Utility.h"


FFullScreenQuad::FFullScreenQuad(FRHI* rhi, FRHIVertexLayout* layout) :
    mRHI(rhi),
    mLayout(layout)
{
}

FFullScreenQuad::~FFullScreenQuad()
{
}


void FFullScreenQuad::Init()
{
    const FScreenVertex vertexes[4] = { FScreenVertex(FVector3(-1.f, -1.f, 0.0f), FVector2(0.0f, 1.0f)),
        FScreenVertex(FVector3(1.f, -1.f, 0.0f), FVector2(1.0f, 1.0f)),
        FScreenVertex(FVector3(-1.f, 1.f, 0.0f), FVector2(0.0f, 0.0f)),
        FScreenVertex(FVector3(1.f, 1.f, 0.0f), FVector2(1.0f, 0.0f)) };
    const uint32 indexes[6] = { 1, 2, 0, 3, 2, 1 };

    VertexBuffer = mRHI->CreateVertexBuffer(sizeof(FScreenVertex), 4, (uint8*)vertexes);
    IndexBuffer = mRHI->CreateIndexBuffer(sizeof(uint32), 6, (uint8*)indexes);

    FShaderInfo vertexShaderInfo;
    vertexShaderInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Postprocess.hlsl");
    vertexShaderInfo.EnterPoint = "PostprocessVS";
    vertexShaderInfo.Target = "vs_5_0";

    VertexShader = TSingleton<FShaderManager>::GetInstance().GetOrCreate(vertexShaderInfo);

    FShaderInfo pixelShaderInfo;
    pixelShaderInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Postprocess.hlsl");
    pixelShaderInfo.EnterPoint = "PostprocessPS";
    pixelShaderInfo.Target = "ps_5_0";

    PixelShader = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderInfo);

    FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
    FPipelineStateInfo info;
    info.ShaderBindings = shaderBindings;
    info.VertexShader = VertexShader;
    info.PixelShader = PixelShader;
    info.VertexLayout = mLayout;
    info.DepthStencilState.bEnableDepthWrite = false;
    info.DepthStencilState.DepthTest = CF_Always;
    info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

}

void FFullScreenQuad::UnInit()
{
    delete VertexBuffer;
    VertexBuffer = nullptr;

    delete IndexBuffer;
    IndexBuffer = nullptr;

    VertexShader = nullptr;

    PixelShader = nullptr;

}

void FFullScreenQuad::Draw()
{
}

FPostProcessing::FPostProcessing(FRHI* rhi, FRHIRenderTarget* sceneColor, FRHIRenderTarget* renderTarget) : 
mRHI(rhi),
mSceneColor(sceneColor),
mRenderTarget(renderTarget),
mPostProcessConstantBuffer(nullptr),
mFullScreenQuad(nullptr)
{
    BloomIntensity = 0.675f;
    BloomThreshold = -1.0f;

    Bloom1Tint = FLinearColor(0.3465f, 0.3465f, 0.3465f, 1.0f);
    Bloom2Tint = FLinearColor(0.138f, 0.138f, 0.138f, 1.0f);
    Bloom3Tint = FLinearColor(0.1176f, 0.1176f, 0.1176f, 1.0f);
    Bloom4Tint = FLinearColor(0.066f, 0.066f, 0.066f, 1.0f);
    Bloom5Tint = FLinearColor(0.066f, 0.066f, 0.066f, 1.0f);
    Bloom6Tint = FLinearColor(0.061f, 0.061f, 0.061f, 1.0f);
}

FPostProcessing::~FPostProcessing()
{
}

void FPostProcessing::Init()
{
    mFullScreenLayout = new FRHIVertexLayout;

    FInputElementDesc inputLayout[] = {
        { "POSITION", 0, EPixelFormat::PF_R32G32B32_FLOAT, 0, 0,  ICF_PerVertexData, 0 },
        { "TEXCOORD", 0, EPixelFormat::PF_R32G32_FLOAT, 0, 12, ICF_PerVertexData, 0 } };

    mFullScreenLayout->Elements.push_back(inputLayout[0]);
    mFullScreenLayout->Elements.push_back(inputLayout[1]);

    mFullScreenQuad = new FFullScreenQuad(mRHI, mFullScreenLayout);
    mFullScreenQuad->Init();

    const int32 sceneColorWidth = TSingleton<FConfigManager>::GetInstance().WindowWidth;
    const int32 sceneColorHeight = TSingleton<FConfigManager>::GetInstance().WindowHeight;

    const float bloomSetupScale = 0.25f;
    const int32 bloomSetupWidth = static_cast<int32>(sceneColorWidth * 0.25f);
    const int32 bloomSetupHeight = static_cast<int32>(sceneColorHeight * 0.25f);
    mBloomSetup = mRHI->CreateRenderTarget(bloomSetupWidth,
        bloomSetupHeight, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    const int32 bloomDown0Width = static_cast<int32>(bloomSetupWidth * 0.5f);
    const int32 bloomDown0Height = static_cast<int32>(bloomSetupHeight * 0.5f);
    mBloomDown0 = mRHI->CreateRenderTarget(bloomDown0Width,
        bloomDown0Height, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    const int32 bloomDown1Width = static_cast<int32>(bloomDown0Width * 0.5f);
    const int32 bloomDown1Height = static_cast<int32>(bloomDown0Height * 0.5f);
    mBloomDown1 = mRHI->CreateRenderTarget(bloomDown1Width,
        bloomDown1Height, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    const int32 bloomDown2Width = static_cast<int32>(bloomDown1Width * 0.5f);
    const int32 bloomDown2Height = static_cast<int32>(bloomDown1Height * 0.5f);
    mBloomDown2 = mRHI->CreateRenderTarget(bloomDown2Width,
        bloomDown2Height, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    const int32 bloomDown3Width = static_cast<int32>(bloomDown2Width * 0.5f);
    const int32 bloomDown3Height = static_cast<int32>(bloomDown2Height * 0.5f);
    mBloomDown3 = mRHI->CreateRenderTarget(bloomDown3Width,
        bloomDown3Height, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    const int32 bloomUp0Width = bloomDown3Width * 2;
    const int32 bloomUp0Height = bloomDown3Height * 2;
    mBloomUp0 = mRHI->CreateRenderTarget(bloomUp0Width,
        bloomUp0Height, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    const int32 bloomUp1Width = bloomUp0Width * 2;
    const int32 bloomUp1Height = bloomUp0Height * 2;
    mBloomUp1 = mRHI->CreateRenderTarget(bloomUp1Width,
        bloomUp1Height, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    const int32 bloomUp2Width = bloomUp1Width * 2;
    const int32 bloomUp2Height = bloomUp1Height * 2;
    mBloomUp2 = mRHI->CreateRenderTarget(bloomUp2Width,
        bloomUp2Height, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    const int32 bloomUp3Width = bloomUp2Width * 2;
    const int32 bloomUp3Height = bloomUp2Height * 2;
    mBloomUp3 = mRHI->CreateRenderTarget(bloomUp3Width,
        bloomUp3Height, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    mToneMap = mRHI->CreateRenderTarget(sceneColorWidth,
        sceneColorHeight, 1, PF_R16G16B16A16_FLOAT, PF_UNKNOWN);

    mBloomUpLastResault = mBloomDown3;
    mBloomUpSource = mBloomDown3;
    mBloomDownSource = mBloomUp0;

    creatPostProcessConstantBuffer();

    TMap<FString, FString> defines;
    {
        FShaderInfo vertexShaderBloomSetupInfo;
        vertexShaderBloomSetupInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Bloom.hlsl");
        vertexShaderBloomSetupInfo.EnterPoint = "BloomSetupVS";
        vertexShaderBloomSetupInfo.Target = "vs_5_0";

        VertexShaderBloomSetup = TSingleton<FShaderManager>::GetInstance().GetOrCreate(vertexShaderBloomSetupInfo);

        FShaderInfo pixelShaderBloomSetupInfo;
        pixelShaderBloomSetupInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Bloom.hlsl");
        pixelShaderBloomSetupInfo.EnterPoint = "BloomSetupPS";
        pixelShaderBloomSetupInfo.Target = "ps_5_0";

        PixelShaderBloomSetup = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderBloomSetupInfo);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomSetup;
        info.PixelShader = PixelShaderBloomSetup;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    }

    {
        FShaderInfo vertexShaderBloomDownInfo;
        vertexShaderBloomDownInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Bloom.hlsl");
        vertexShaderBloomDownInfo.EnterPoint = "BloomDownVS";
        vertexShaderBloomDownInfo.Target = "vs_5_0";

        VertexShaderBloomDown = TSingleton<FShaderManager>::GetInstance().GetOrCreate(vertexShaderBloomDownInfo);

        FShaderInfo pixelShaderBloomDownInfo;
        pixelShaderBloomDownInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Bloom.hlsl");
        pixelShaderBloomDownInfo.EnterPoint = "BloomDownPS";
        pixelShaderBloomDownInfo.Target = "ps_5_0";

        PixelShaderBloomDown = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderBloomDownInfo);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    }

    {
        FShaderInfo vertexShaderBloomUpInfo;
        vertexShaderBloomUpInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Bloom.hlsl");
        vertexShaderBloomUpInfo.EnterPoint = "BloomUpVS";
        vertexShaderBloomUpInfo.Target = "vs_5_0";

        VertexShaderBloomUp = TSingleton<FShaderManager>::GetInstance().GetOrCreate(vertexShaderBloomUpInfo);

        FShaderInfo PixelShaderBloomUpInfo;
        PixelShaderBloomUpInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Bloom.hlsl");
        PixelShaderBloomUpInfo.EnterPoint = "BloomUpPS";
        PixelShaderBloomUpInfo.Target = "ps_5_0";

        PixelShaderBloomUp = TSingleton<FShaderManager>::GetInstance().GetOrCreate(PixelShaderBloomUpInfo);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    }

    {
        FShaderInfo vertexShaderTonemapInfo;
        vertexShaderTonemapInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Tonemap.hlsl");
        vertexShaderTonemapInfo.EnterPoint = "TonemapVS";
        vertexShaderTonemapInfo.Target = "vs_5_0";

        VertexShaderTonemap = TSingleton<FShaderManager>::GetInstance().GetOrCreate(vertexShaderTonemapInfo);

        FShaderInfo pixelShaderTonemapInfo;
        pixelShaderTonemapInfo.FilePathName = StringToWString(FConfigManager::ShaderPath + "Tonemap.hlsl");
        pixelShaderTonemapInfo.EnterPoint = "TonemapPS";
        pixelShaderTonemapInfo.Target = "ps_5_0";

        PixelShaderTonemap = TSingleton<FShaderManager>::GetInstance().GetOrCreate(pixelShaderTonemapInfo);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderTonemap;
        info.PixelShader = PixelShaderTonemap;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R8G8B8A8_UNORM;

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    }
}

void FPostProcessing::UnInit()
{
    mBloomSetup->UnInit();
    delete mBloomSetup;
    mBloomSetup = nullptr;

    mBloomDown0->UnInit();
    delete mBloomDown0;
    mBloomDown0 = nullptr;

    mBloomDown1->UnInit();
    delete mBloomDown1;
    mBloomDown1 = nullptr;

    mBloomDown2->UnInit();
    delete mBloomDown2;
    mBloomDown2 = nullptr;

    mBloomDown3->UnInit();
    delete mBloomDown3;
    mBloomDown3 = nullptr;

    mBloomUp0->UnInit();
    delete mBloomUp0;
    mBloomUp0 = nullptr;

    mBloomUp1->UnInit();
    delete mBloomUp1;
    mBloomUp1 = nullptr;

    mBloomUp2->UnInit();
    delete mBloomUp2;
    mBloomUp2 = nullptr;

    mBloomUp3->UnInit();
    delete mBloomUp3;
    mBloomUp3 = nullptr;

    mToneMap->UnInit();
    delete mToneMap;
    mToneMap = nullptr;

    mFullScreenQuad->UnInit();
    delete mFullScreenQuad;
    mFullScreenQuad = nullptr;

    VertexShaderBloomSetup = nullptr;

    PixelShaderBloomSetup = nullptr;

    VertexShaderBloomDown = nullptr;

    PixelShaderBloomDown = nullptr;

    VertexShaderBloomUp = nullptr;

    PixelShaderBloomUp = nullptr;

    VertexShaderTonemap = nullptr;

    PixelShaderTonemap = nullptr;

    delete mFullScreenLayout;
    mFullScreenLayout = nullptr;

    mPostProcessConstantBuffer->UnInit();
    delete mPostProcessConstantBuffer;
    mPostProcessConstantBuffer = nullptr;

    mBloomDown0ConstantBuffer->UnInit();
    delete mBloomDown0ConstantBuffer;
    mBloomDown0ConstantBuffer = nullptr;

    mBloomDown1ConstantBuffer->UnInit();
    delete mBloomDown1ConstantBuffer;
    mBloomDown1ConstantBuffer = nullptr;

    mBloomDown2ConstantBuffer->UnInit();
    delete mBloomDown2ConstantBuffer;
    mBloomDown2ConstantBuffer = nullptr;

    mBloomDown3ConstantBuffer->UnInit();
    delete mBloomDown3ConstantBuffer;
    mBloomDown3ConstantBuffer = nullptr;

    mBloomUp0ConstantBuffer->UnInit();
    delete mBloomUp0ConstantBuffer;
    mBloomUp0ConstantBuffer = nullptr;

    mBloomUp1ConstantBuffer->UnInit();
    delete mBloomUp1ConstantBuffer;
    mBloomUp1ConstantBuffer = nullptr;

    mBloomUp2ConstantBuffer->UnInit();
    delete mBloomUp2ConstantBuffer;
    mBloomUp2ConstantBuffer = nullptr;

    mBloomUp3ConstantBuffer->UnInit();
    delete mBloomUp3ConstantBuffer;
    mBloomUp3ConstantBuffer = nullptr;

    mBloomUpLastResault = nullptr;
    mBloomUpSource = nullptr;
    mBloomDownSource = nullptr;
}

void FPostProcessing::Draw()
{
    mRHI->BeginEvent("PostProcess");

    updatePostProcessConstantBuffer();

    mRHI->BeginEvent("BloomSetup");
    {
        mRHI->SetRenderTarget(mBloomSetup);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mBloomSetup->Width), static_cast<float>(mBloomSetup->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mBloomSetup->Width), static_cast<float>(mBloomSetup->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mBloomSetup->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomSetup;
        info.PixelShader = PixelShaderBloomSetup;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture(mSceneColor->RenderTargets[0], 0);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mBloomSetup->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);

    }
    mRHI->EndEvent();

    mRHI->BeginEvent("BloomDown0");
    {
        mRHI->SetRenderTarget(mBloomDown0);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mBloomDown0->Width), static_cast<float>(mBloomDown0->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mBloomDown0->Width), static_cast<float>(mBloomDown0->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mBloomDown0->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mBloomDown0ConstantBuffer, 0);
        mRHI->SetTexture(mBloomSetup->RenderTargets[0], 0);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mBloomDown0->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);

    }
    mRHI->EndEvent();

    mRHI->BeginEvent("BloomDown1");
    {
        mRHI->SetRenderTarget(mBloomDown1);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mBloomDown1->Width), static_cast<float>(mBloomDown1->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mBloomDown1->Width), static_cast<float>(mBloomDown1->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mBloomDown1->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mBloomDown1ConstantBuffer, 0);
        mRHI->SetTexture(mBloomDown0->RenderTargets[0], 0);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mBloomDown1->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);

    }
    mRHI->EndEvent();


    mRHI->BeginEvent("BloomDown2");
    {
        mRHI->SetRenderTarget(mBloomDown2);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mBloomDown2->Width), static_cast<float>(mBloomDown2->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mBloomDown2->Width), static_cast<float>(mBloomDown2->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mBloomDown2->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mBloomDown2ConstantBuffer, 0);
        mRHI->SetTexture(mBloomDown1->RenderTargets[0], 0);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mBloomDown2->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);

    }
    mRHI->EndEvent();

    mRHI->BeginEvent("BloomDown3");
    {
        mRHI->SetRenderTarget(mBloomDown3);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mBloomDown3->Width), static_cast<float>(mBloomDown3->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mBloomDown3->Width), static_cast<float>(mBloomDown3->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mBloomDown3->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mBloomDown3ConstantBuffer, 0);
        mRHI->SetTexture(mBloomDown2->RenderTargets[0], 0);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mBloomDown3->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);

    }
    mRHI->EndEvent();

    mRHI->BeginEvent("BloomUp0");
    {
        mRHI->SetRenderTarget(mBloomUp0);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mBloomUp0->Width), static_cast<float>(mBloomUp0->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mBloomUp0->Width), static_cast<float>(mBloomUp0->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mBloomUp0->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mBloomUp0ConstantBuffer, 0);
        mRHI->SetTexture(mBloomDown2->RenderTargets[0], 0);
        mRHI->SetTexture(mBloomDown3->RenderTargets[0], 1);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mBloomUp0->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);

    }
    mRHI->EndEvent();

    mRHI->BeginEvent("BloomUp1");
    {
        mRHI->SetRenderTarget(mBloomUp1);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mBloomUp1->Width), static_cast<float>(mBloomUp1->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mBloomUp1->Width), static_cast<float>(mBloomUp1->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mBloomUp1->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mBloomUp1ConstantBuffer, 0);
        mRHI->SetTexture(mBloomDown1->RenderTargets[0], 0);
        mRHI->SetTexture(mBloomUp0->RenderTargets[0], 1);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mBloomUp1->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);

    }
    mRHI->EndEvent();


    mRHI->BeginEvent("BloomUp2");
    {
        mRHI->SetRenderTarget(mBloomUp2);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mBloomUp2->Width), static_cast<float>(mBloomUp2->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mBloomUp2->Width), static_cast<float>(mBloomUp2->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mBloomUp2->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mBloomUp2ConstantBuffer, 0);
        mRHI->SetTexture(mBloomDown0->RenderTargets[0], 0);
        mRHI->SetTexture(mBloomUp1->RenderTargets[0], 1);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mBloomUp2->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);

    }
    mRHI->EndEvent();


    mRHI->BeginEvent("BloomUp3");
    {
        mRHI->SetRenderTarget(mBloomUp3);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mBloomUp3->Width), static_cast<float>(mBloomUp3->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mBloomUp3->Width), static_cast<float>(mBloomUp3->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mBloomUp3->RenderTargets[0], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R16G16B16A16_FLOAT;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mBloomUp3ConstantBuffer, 0);
        mRHI->SetTexture(mBloomSetup->RenderTargets[0], 0);
        mRHI->SetTexture(mBloomUp2->RenderTargets[0], 1);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mBloomUp3->RenderTargets[0], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);

    }
    mRHI->EndEvent();


    mRHI->BeginEvent("ToneMap");
    {
        mRHI->SetRenderTarget(mRenderTarget);

        mRHI->SetViewPort(0.0f, 0.0f, 0.0f, static_cast<float>(mRenderTarget->Width), static_cast<float>(mRenderTarget->Height), 1.0f);
        mRHI->SetSetScissor(true, 0.0f, 0.0f, static_cast<float>(mRenderTarget->Width), static_cast<float>(mRenderTarget->Height));

        const FRHITransitionInfo infoRenderTargetBegin(mRenderTarget->RenderTargets[mRenderTarget->GetRenderTargetIndex()], ACCESS_PRESENT, ACCESS_RENDER_TARGET);
        mRHI->TransitionResource(infoRenderTargetBegin);
        const FRHITransitionInfo infoDepthStencilBegin(mRenderTarget->DepthStencilTarget, ACCESS_COMMON, ACCESS_DEPTH_WRITE);
        mRHI->TransitionResource(infoDepthStencilBegin);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetShaderBindings();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderTonemap;
        info.PixelShader = PixelShaderTonemap;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;
        info.DepthStencilState.DepthTest = CF_Always;
        info.RenderTargetFormat = EPixelFormat::PF_R8G8B8A8_UNORM;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture(mSceneColor->RenderTargets[0], 0);
        mRHI->SetTexture(mBloomUp3->RenderTargets[0], 1);

        mRHI->DrawIndexedInstanced(6, 1, 0, 0, 0);

        const FRHITransitionInfo infoRenderTargetEnd(mRenderTarget->RenderTargets[mRenderTarget->GetRenderTargetIndex()], ACCESS_RENDER_TARGET, ACCESS_PRESENT);
        mRHI->TransitionResource(infoRenderTargetEnd);
        const FRHITransitionInfo infoDepthStencilEnd(mRenderTarget->DepthStencilTarget, ACCESS_DEPTH_WRITE, ACCESS_COMMON);
        mRHI->TransitionResource(infoDepthStencilEnd);

    }
    mRHI->EndEvent();

    mRHI->EndEvent();

}

void FPostProcessing::creatPostProcessConstantBuffer()
{
    FPostProcessConstant postProcessConstant;
    _createPostProcessConstant(postProcessConstant);

    mPostProcessConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FPostProcessConstant), (uint8*)&postProcessConstant);

    FBloomDownConstant bloomDownConstant0;
    mBloomDownSource = mBloomSetup;
    _createBloomDownConstant(bloomDownConstant0);
    mBloomDown0ConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FBloomDownConstant), (uint8*)&bloomDownConstant0);

    FBloomDownConstant bloomDownConstant1;
    mBloomDownSource = mBloomDown0;
    _createBloomDownConstant(bloomDownConstant1);
    mBloomDown1ConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FBloomDownConstant), (uint8*)&bloomDownConstant1);

    FBloomDownConstant bloomDownConstant2;
    mBloomDownSource = mBloomDown1;
    _createBloomDownConstant(bloomDownConstant2);
    mBloomDown2ConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FBloomDownConstant), (uint8*)&bloomDownConstant2);

    FBloomDownConstant bloomDownConstant3;
    mBloomDownSource = mBloomDown2;
    _createBloomDownConstant(bloomDownConstant3);
    mBloomDown3ConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FBloomDownConstant), (uint8*)&bloomDownConstant3);

    FBloomUpConstant bloomUpConstant0;
    mBloomUpLastResault = mBloomDown3;
    mBloomUpSource = mBloomDown2;
    _createBloomUpConstant(bloomUpConstant0);
    bloomUpConstant0.BloomTintA = Bloom4Tint;
    bloomUpConstant0.BloomTintB = Bloom5Tint;
    bloomUpConstant0.BloomTintA *= BloomIntensity;
    bloomUpConstant0.BloomTintB *= BloomIntensity;

    bloomUpConstant0.BloomTintA *= 1.0f / 8.0f;
    bloomUpConstant0.BloomTintB *= 1.0f / 8.0f;

    mBloomUp0ConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FBloomUpConstant), (uint8*)&bloomUpConstant0);

    FBloomUpConstant bloomUpConstant1;
    mBloomUpLastResault = mBloomUp0;
    mBloomUpSource = mBloomDown1;
    _createBloomUpConstant(bloomUpConstant1);
    bloomUpConstant1.BloomTintA = Bloom3Tint;
    bloomUpConstant1.BloomTintB = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
    bloomUpConstant1.BloomTintA *= BloomIntensity;

    bloomUpConstant1.BloomTintA *= 1.0f / 8.0f;
    bloomUpConstant1.BloomTintB *= 1.0f / 8.0f;

    mBloomUp1ConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FBloomUpConstant), (uint8*)&bloomUpConstant1);

    FBloomUpConstant bloomUpConstant2;
    mBloomUpLastResault = mBloomUp1;
    mBloomUpSource = mBloomDown0;
    bloomUpConstant2.BloomTintA = Bloom2Tint;
    bloomUpConstant2.BloomTintB = FVector4(1.0, 1.0f, 1.0f, 1.0f);
    bloomUpConstant2.BloomTintA *= BloomIntensity;
    bloomUpConstant2.BloomTintA *= 0.5f;

    bloomUpConstant2.BloomTintA *= 1.0f / 8.0f;
    bloomUpConstant2.BloomTintB *= 1.0f / 8.0f;

    _createBloomUpConstant(bloomUpConstant2);
    mBloomUp2ConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FBloomUpConstant), (uint8*)&bloomUpConstant2);

    FBloomUpConstant bloomUpConstant3;
    mBloomUpLastResault = mBloomUp2;
    mBloomUpSource = mBloomSetup;
    _createBloomUpConstant(bloomUpConstant3);
    float Scale3 = 1.0f / 5.0f;
    bloomUpConstant3.BloomTintA = Bloom1Tint;
    bloomUpConstant3.BloomTintB = FVector4(1.0, 1.0f, 1.0f, 1.0f) * Scale3;
    bloomUpConstant3.BloomTintA *= BloomIntensity;
    bloomUpConstant3.BloomTintA *= 0.5f;
    bloomUpConstant3.BloomTintA *= Scale3;

    bloomUpConstant3.BloomTintA *= 1.0f / 8.0f;
    bloomUpConstant3.BloomTintB *= 1.0f / 8.0f;

    mBloomUp3ConstantBuffer = mRHI->CreateConstantBuffer(sizeof(FBloomUpConstant), (uint8*)&bloomUpConstant3);

}

void FPostProcessing::updatePostProcessConstantBuffer()
{
    FPostProcessConstant postProcessConstant;
    _createPostProcessConstant(postProcessConstant);

    mRHI->UpdateConstantBuffer(mPostProcessConstantBuffer, sizeof(FPostProcessConstant), (uint8*)&postProcessConstant);
}

void FPostProcessing::_createPostProcessConstant(FPostProcessConstant& constant)
{
    constant.SceneColorInvSize = FVector2(1.0f / mSceneColor->Width, 1.0f / mSceneColor->Height);
    constant.BloomThreshold = BloomThreshold;
    constant.BloomColor = FVector3(Bloom1Tint[0], Bloom1Tint[1], Bloom1Tint[2]) * BloomIntensity * 0.5f;
}

void FPostProcessing::_createBloomDownConstant(FBloomDownConstant& constant)
{
    constant.BloomDownInvSize = FVector2(1.0f / mBloomDownSource->Width, 1.0f / mBloomDownSource->Height);
    float BloomDownScale = 0.66f * 4.0f;
    constant.BloomDownScale = BloomDownScale;
}

void FPostProcessing::_createBloomUpConstant(FBloomUpConstant& constant)
{
    constant.BloomLastResaultInvSize = FVector2(1.0f / mBloomUpLastResault->Width, 1.0f / mBloomUpLastResault->Height);
    constant.BloomUpSourceInvSize = FVector2(1.0f / mBloomUpSource->Width, 1.0f / mBloomUpSource->Height);
    float BloomUpScale = 0.66f * 2.0f;
    constant.BloomUpScales = FVector2(BloomUpScale, BloomUpScale);
}
