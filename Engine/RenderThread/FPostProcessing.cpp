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
    const uint16 indexes[6] = { 0, 2, 1, 1, 2, 3 };

    VertexBuffer = mRHI->CreateVertexBuffer(sizeof(FScreenVertex), 4, (uint8*)vertexes);
    IndexBuffer = mRHI->CreateIndexBuffer(sizeof(uint16), 6, (uint8*)indexes);

    const std::wstring vsFilePathName = L"Engine\\Shader\\Postprocess.hlsl";
    const std::string vsEnterPoint = "PostprocessVS";
    const std::string vsTarget = "vs_5_0";

    VertexShader = mRHI->CreateShader(vsFilePathName, vsEnterPoint, vsTarget);

    const std::wstring psFilePathName = L"Engine\\Shader\\Postprocess.hlsl";
    const std::string psEnterPoint = "PostprocessPS";
    const std::string psTarget = "ps_5_0";

    PixelShader = mRHI->CreateShader(psFilePathName, psEnterPoint, psTarget);

    FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();
    FPipelineStateInfo info;
    info.ShaderBindings = shaderBindings;
    info.VertexShader = VertexShader;
    info.PixelShader = PixelShader;
    info.VertexLayout = mLayout;
    info.DepthStencilState.bEnableDepthWrite = false;

    TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

}

void FFullScreenQuad::UnInit()
{
    delete VertexBuffer;
    VertexBuffer = nullptr;

    delete IndexBuffer;
    IndexBuffer = nullptr;

    delete VertexShader;
    VertexShader = nullptr;

    delete PixelShader;
    PixelShader = nullptr;

}

void FFullScreenQuad::Draw()
{
}

FPostProcessing::FPostProcessing(FRHI* rhi, FRHIRenderTarget* sceneColor, FRHIRenderTarget* renderTarget) : mRHI(rhi),
mSceneColor(sceneColor),
mRenderTarget(renderTarget),
mPostProcessConstantBuffer(nullptr),
mFullScreenQuad(nullptr)
{
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
        bloomSetupHeight, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    const int32 bloomDown0Width = static_cast<int32>(bloomSetupWidth * 0.5f);
    const int32 bloomDown0Height = static_cast<int32>(bloomSetupHeight * 0.5f);
    mBloomDown0 = mRHI->CreateRenderTarget(bloomDown0Width,
        bloomDown0Height, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    const int32 bloomDown1Width = static_cast<int32>(bloomDown0Width * 0.5f);
    const int32 bloomDown1Height = static_cast<int32>(bloomDown0Height * 0.5f);
    mBloomDown1 = mRHI->CreateRenderTarget(bloomDown1Width,
        bloomDown1Height, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    const int32 bloomDown2Width = static_cast<int32>(bloomDown1Width * 0.5f);
    const int32 bloomDown2Height = static_cast<int32>(bloomDown1Height * 0.5f);
    mBloomDown2 = mRHI->CreateRenderTarget(bloomDown2Width,
        bloomDown2Height, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    const int32 bloomDown3Width = static_cast<int32>(bloomDown2Width * 0.5f);
    const int32 bloomDown3Height = static_cast<int32>(bloomDown2Height * 0.5f);
    mBloomDown3 = mRHI->CreateRenderTarget(bloomDown3Width,
        bloomDown3Height, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    const int32 bloomUp0Width = bloomDown3Width * 2;
    const int32 bloomUp0Height = bloomDown3Width * 2;
    mBloomUp0 = mRHI->CreateRenderTarget(bloomUp0Width,
        bloomUp0Height, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    const int32 bloomUp1Width = bloomUp0Width * 2;
    const int32 bloomUp1Height = bloomUp0Height * 2;
    mBloomUp1 = mRHI->CreateRenderTarget(bloomUp1Width,
        bloomUp1Height, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    const int32 bloomUp2Width = bloomUp1Width * 2;
    const int32 bloomUp2Height = bloomUp1Height * 2;
    mBloomUp2 = mRHI->CreateRenderTarget(bloomUp2Width,
        bloomUp2Height, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    const int32 bloomUp3Width = bloomUp2Width * 2;
    const int32 bloomUp3Height = bloomUp2Height * 2;
    mBloomUp3 = mRHI->CreateRenderTarget(bloomUp3Width,
        bloomUp3Height, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    mToneMap = mRHI->CreateRenderTarget(sceneColorWidth,
        sceneColorHeight, 1, PF_R8G8B8A8_UNORM, PF_UNKNOWN);

    creatPostProcessConstantBuffer();

    {
        const std::wstring vsFilePathName = L"Engine\\Shader\\Bloom.hlsl";
        const std::string vsEnterPoint = "BloomSetupVS";
        const std::string vsTarget = "vs_5_0";

        VertexShaderBloomSetup = mRHI->CreateShader(vsFilePathName, vsEnterPoint, vsTarget);

        const std::wstring psFilePathName = L"Engine\\Shader\\Bloom.hlsl";
        const std::string psEnterPoint = "BloomSetupPS";
        const std::string psTarget = "ps_5_0";

        PixelShaderBloomSetup = mRHI->CreateShader(psFilePathName, psEnterPoint, psTarget);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomSetup;
        info.PixelShader = PixelShaderBloomSetup;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    }

    {
        const std::wstring vsFilePathName = L"Engine\\Shader\\Bloom.hlsl";
        const std::string vsEnterPoint = "BloomDownVS";
        const std::string vsTarget = "vs_5_0";

        VertexShaderBloomDown = mRHI->CreateShader(vsFilePathName, vsEnterPoint, vsTarget);

        const std::wstring psFilePathName = L"Engine\\Shader\\Bloom.hlsl";
        const std::string psEnterPoint = "BloomDownPS";
        const std::string psTarget = "ps_5_0";

        PixelShaderBloomDown = mRHI->CreateShader(psFilePathName, psEnterPoint, psTarget);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    }

    {
        const std::wstring vsFilePathName = L"Engine\\Shader\\Bloom.hlsl";
        const std::string vsEnterPoint = "BloomUpVS";
        const std::string vsTarget = "vs_5_0";

        VertexShaderBloomUp = mRHI->CreateShader(vsFilePathName, vsEnterPoint, vsTarget);

        const std::wstring psFilePathName = L"Engine\\Shader\\Bloom.hlsl";
        const std::string psEnterPoint = "BloomUpPS";
        const std::string psTarget = "ps_5_0";

        PixelShaderBloomUp = mRHI->CreateShader(psFilePathName, psEnterPoint, psTarget);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        TSingleton<FPipelineStateManager>::GetInstance().CreatePipleLineState(info);

    }

    {
        const std::wstring vsFilePathName = L"Engine\\Shader\\Bloom.hlsl";
        const std::string vsEnterPoint = "BloomMergeVS";
        const std::string vsTarget = "vs_5_0";

        VertexShaderBloomMerge = mRHI->CreateShader(vsFilePathName, vsEnterPoint, vsTarget);

        const std::wstring psFilePathName = L"Engine\\Shader\\Bloom.hlsl";
        const std::string psEnterPoint = "BloomMergePS";
        const std::string psTarget = "ps_5_0";

        PixelShaderBloomMerge = mRHI->CreateShader(psFilePathName, psEnterPoint, psTarget);

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomMerge;
        info.PixelShader = PixelShaderBloomMerge;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

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

    delete VertexShaderBloomSetup;
    VertexShaderBloomSetup = nullptr;

    delete PixelShaderBloomSetup;
    PixelShaderBloomSetup = nullptr;

    delete VertexShaderBloomDown;
    VertexShaderBloomDown = nullptr;

    delete PixelShaderBloomDown;
    PixelShaderBloomDown = nullptr;

    delete VertexShaderBloomUp;
    VertexShaderBloomUp = nullptr;

    delete PixelShaderBloomUp;
    PixelShaderBloomUp = nullptr;

    delete VertexShaderBloomMerge;
    VertexShaderBloomMerge = nullptr;

    delete PixelShaderBloomMerge;
    PixelShaderBloomMerge = nullptr;

    delete mFullScreenLayout;
    mFullScreenLayout = nullptr;

    mPostProcessConstantBuffer->UnInit();
    delete mPostProcessConstantBuffer;
    mPostProcessConstantBuffer = nullptr;

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomSetup;
        info.PixelShader = PixelShaderBloomSetup;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mSceneColor->RenderTargets[0], 0);

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();
        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mBloomSetup->RenderTargets[0], 0);

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mBloomDown0->RenderTargets[0], 0);

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mBloomDown1->RenderTargets[0], 0);

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomDown;
        info.PixelShader = PixelShaderBloomDown;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mBloomDown2->RenderTargets[0], 0);

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mBloomDown3->RenderTargets[0], 0);
        mRHI->SetTexture2D(mBloomDown2->RenderTargets[0], 1);

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mBloomUp0->RenderTargets[0], 0);
        mRHI->SetTexture2D(mBloomDown1->RenderTargets[0], 1);

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mBloomUp1->RenderTargets[0], 0);
        mRHI->SetTexture2D(mBloomDown0->RenderTargets[0], 1);

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomUp;
        info.PixelShader = PixelShaderBloomUp;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mBloomUp2->RenderTargets[0], 0);
        mRHI->SetTexture2D(mBloomSetup->RenderTargets[0], 1);

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

        FRHIShaderBindings* shaderBindings = TSingleton<FShaderBindingsManager>::GetInstance().GetRootSignature();

        FPipelineStateInfo info;
        info.ShaderBindings = shaderBindings;
        info.VertexShader = VertexShaderBloomMerge;
        info.PixelShader = PixelShaderBloomMerge;
        info.VertexLayout = mFullScreenLayout;
        info.DepthStencilState.bEnableDepthWrite = false;

        FRHIPipelineState* pipelineState = TSingleton<FPipelineStateManager>::GetInstance().GetPipleLineState(info);

        mRHI->SetPipelineState(pipelineState);
        mRHI->SetPrimitiveType(EPrimitiveType::PT_TriangleList);
        mRHI->SetVertexBuffer(mFullScreenQuad->VertexBuffer);
        mRHI->SetIndexBuffer(mFullScreenQuad->IndexBuffer);
        mRHI->SetConstantBuffer(mPostProcessConstantBuffer, 0);
        mRHI->SetTexture2D(mSceneColor->RenderTargets[0], 0);
        mRHI->SetTexture2D(mBloomUp3->RenderTargets[0], 1);

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
    constant.BloomThreshold = -1.0f;
    constant.BloomDownScale = 0.5f;
    constant.BloomUpScales = FVector2(2.0f, 2.0f);
    constant.BloomLastResaultInvSize = FVector2(1.0f / mBloomUp1->Width, 1.0f / mBloomUp1->Height);
    constant.BloomDownInvSize = FVector2(1.0f / mBloomDown0->Width, 1.0f / mBloomDown0->Height);
    constant.BloomUpInvSize = FVector2(1.0f / mBloomUp1->Width, 1.0f / mBloomUp1->Height);
    constant.BloomTintA = FVector4(1.0f, 1.0f, 1.0f, 1.0f) / 16.0f;
    constant.BloomTintB = FVector4(1.0f, 1.0f, 1.0f, 1.0f) / 16.0f;
    constant.BloomColor = FVector3(1.0f, 1.0f, 1.0f);
}
