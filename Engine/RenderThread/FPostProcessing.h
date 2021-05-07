#pragma once
#include "Prerequisite.h"


struct FPostProcessConstant
{
    FVector2 SceneColorInvSize;
    float BloomThreshold;
    float BloomDownScale;
    FVector2 BloomUpScales;
    FVector2 BloomLastResaultInvSize;
    FVector2 BloomDownInvSize;
    FVector2 BloomUpInvSize;
    FVector4 BloomTintA;
    FVector4 BloomTintB;
    FVector3 BloomColor;
};

class FFullScreenQuad
{
public:
    FFullScreenQuad(FRHI* rhi, FRHIVertexLayout* layout);
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
    FRHIVertexLayout* mLayout;

};


class FPostProcessing
{
public:
    FPostProcessing(FRHI* rhi, FRHIRenderTarget* sceneColor, FRHIRenderTarget* renderTarget);
    ~FPostProcessing();

public:
    void Init();
    void UnInit();

    void Draw();

protected:
    void creatPostProcessConstantBuffer();
    void updatePostProcessConstantBuffer();

private:
    void _createPostProcessConstant(FPostProcessConstant& constant);

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

    FRHIShader* VertexShaderBloomSetup;
    FRHIShader* PixelShaderBloomSetup;

    FRHIShader* VertexShaderBloomDown;
    FRHIShader* PixelShaderBloomDown;

    FRHIShader* VertexShaderBloomUp;
    FRHIShader* PixelShaderBloomUp;

    FRHIShader* VertexShaderTonemap;
    FRHIShader* PixelShaderTonemap;

    FRHIVertexLayout* mFullScreenLayout;
    FRHIConstantBuffer* mPostProcessConstantBuffer;

    FRHIRenderTarget* mBloomLastResault;
    FRHIRenderTarget* mBloomDown;
    FRHIRenderTarget* mBloomUp;

};
