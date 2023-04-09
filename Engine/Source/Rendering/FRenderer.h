#pragma once
#include "Prerequisite.h"

#define MAX_POINT_LIGHT_NUM 4

struct FSceneColorPassConstant
{
    FMatrix4x4 View;
    FMatrix4x4 Proj;
    FMatrix4x4 ViewProj;
    FMatrix4x4 ShadowTransform;
    FVector3 CameraPos;
    float _Placeholder0;
    FVector3 CameraDir;
    float _Placeholder1;
    FVector3 DirectionalLightDir;
    float _Placeholder2;
    FVector3 DirectionalLightColor;
    float DirectionalLightIntensity;
    FVector2 InvShadowMapSize;
    float DirectionalLightShadowBias;
    int32 PointLightNum;
    FVector4 PointLightPositionAndInvRadius[MAX_POINT_LIGHT_NUM];
    FVector4 PointLightColorAndFalloffExponent[MAX_POINT_LIGHT_NUM];
    float PointLightIntensity[MAX_POINT_LIGHT_NUM];
};

struct FShadowPassConstant
{
    FMatrix4x4 View;
    FMatrix4x4 Proj;
    FMatrix4x4 ViewProj;
};

class FRenderer
{
public:
    FRenderer(FRHIRenderWindow* renderWindow, FScene* scene, FView* view);
    virtual ~FRenderer();

public:
    virtual void Init();
    virtual void UnInit();

    virtual void Render();

protected:
    virtual void prePass();
    virtual void postPass();

    void initShadowPass();
    void drawShadowPass();
    void unInitShadowPass();
    void creatShadowPassConstantBuffer();
    void updateShadowPassConstantBuffer();

    void initSceneColorPass();
    void drawSceneColorPass();
    void unInitSceneColorPass();
    void createSceneColorPassConstantBuffer();
    void updateSceneColorPassConstantBuffer();

    void initPostProcess();
    void drawPostProcess();
    void unInitPostProcess();

private:
    void _createSceneColorPassConstant(FSceneColorPassConstant& constant);
    void _createShadowPassConstant(FShadowPassConstant& constant);

    void _drawSceneColorOpaque();
    void _drawSceneColorTranslucent();

private:
    FRHI* mRHI;
    FRHIRenderTarget* mRenderTarget;
    FScene* mScene;
    FView* mView;
    FRHIConstantBuffer* mSceneColorPassConstantBuffer;
    FRHIConstantBuffer* mShadowPassConstantBuffer;
    FRHIRenderTarget* mSceneColor;
    FRHIRenderTarget* mSceneColorMS;
    FRHIRenderTarget* mShadowMap;
    FMatrix4x4 mShadowTransform;
    FPostProcessing* mPostProcessing;
};
