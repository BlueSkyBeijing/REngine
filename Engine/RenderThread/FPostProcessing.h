#pragma once
#include "Prerequisite.h"

struct FPostProcessConstant
{
    FVector3 BloomColor;
    float BloomThreshold;
    FVector2 SceneColorInvSize;
    FVector2 _Placeholder0;
};

struct FBloomDownConstant
{
    FVector2 BloomDownInvSize;
    float BloomDownScale;
    float _Placeholder1;
};

struct FBloomUpConstant
{
    FVector4 BloomTintA;
    FVector4 BloomTintB;
    FVector2 BloomLastResaultInvSize;
    FVector2 BloomUpSourceInvSize;
    FVector2 BloomUpScales;
    FVector2 _Placeholder2;
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
    float BloomIntensity;
    float BloomThreshold;
    float BloomSizeScale;

    float Bloom1Size;
    float Bloom2Size;
    float Bloom3Size;
    float Bloom4Size;
    float Bloom5Size;
    float Bloom6Size;

    FLinearColor Bloom1Tint;
    FLinearColor Bloom2Tint;
    FLinearColor Bloom3Tint;
    FLinearColor Bloom4Tint;
    FLinearColor Bloom5Tint;
    FLinearColor Bloom6Tint;

public:
    void Init();
    void UnInit();

    void Draw();

protected:
    void creatPostProcessConstantBuffer();
    void updatePostProcessConstantBuffer();

private:
    void _createPostProcessConstant(FPostProcessConstant& constant);
    void _createBloomDownConstant(FBloomDownConstant& constant);
    void _createBloomUpConstant(FBloomUpConstant& constant);

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

    FRHIConstantBuffer* mBloomDown0ConstantBuffer;
    FRHIConstantBuffer* mBloomDown1ConstantBuffer;
    FRHIConstantBuffer* mBloomDown2ConstantBuffer;
    FRHIConstantBuffer* mBloomDown3ConstantBuffer;

    FRHIConstantBuffer* mBloomUp0ConstantBuffer;
    FRHIConstantBuffer* mBloomUp1ConstantBuffer;
    FRHIConstantBuffer* mBloomUp2ConstantBuffer;
    FRHIConstantBuffer* mBloomUp3ConstantBuffer;

    FRHIRenderTarget* mBloomUpLastResault;
    FRHIRenderTarget* mBloomUpSource;
    FRHIRenderTarget* mBloomDownSource;

};
