#pragma once
#include "Prerequisite.h"

class FFullScreenQuad
{
public:
    FFullScreenQuad(FRHI* rhi);
    ~FFullScreenQuad();

    void Init();
    void UnInit();
    void Draw();

public:
    FRHIVertexBuffer* VertexBuffer;
    FRHIIndexBuffer* IndexBuffer;
    FRHIShader* VertexShader;
    FRHIShader* PixelShader;

private:
    FRHI* mRHI;
};


class FPostProcessing
{
public:
    FPostProcessing(FRHI* rhi, FRHIRenderTarget* sceneColor, FRHIRenderTarget* renderTarget);
    ~FPostProcessing();

    void Init();
    void UnInit();

    void Update();
private:
    FRHI* mRHI;
    FRHIRenderTarget* mBloomSetup;
    FRHIRenderTarget* mBloomDown0;
    FRHIRenderTarget* mBloomDown1;
    FRHIRenderTarget* mBloomDown2;
    FRHIRenderTarget* mBloomDown3;
    FRHIRenderTarget* mBloomUp0;
    FRHIRenderTarget* mBloomUp1;
    FRHIRenderTarget* mBloomUp2;
    FRHIRenderTarget* mBloomUp3;
    FRHIRenderTarget* mToneMap;

    FRHIRenderTarget* mSceneColor;
    FRHIRenderTarget* mRenderTarget;
    FFullScreenQuad* mFullScreenQuad;

    FRHIShader* VertexShaderBloomUp;
    FRHIShader* PixelShaderBloomUp;

};
