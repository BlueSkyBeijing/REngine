#pragma once
#include "Prerequisite.h"

#include "FRHIBuffer.h"

struct FMainPassConstant
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
    float _Placeholder3;
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
    ~FRenderer();

    virtual void Init();
    virtual void UnInit();

    virtual void Render();

protected:
    virtual void preRender();
    virtual void clear();
    virtual void initView();
    virtual void computeVisibility();
    virtual void setRenderTarget(FRHIRenderTarget* renderTarget);
    virtual void setViewPort();
    virtual void drawRenderables();
    virtual void postProcess();
    virtual void postRender();

    void initShadow();
    void updateShadow();
    void unInitShadow();

    void createMainPassConstantBuffer();
    void updateMainPassConstantBuffer();

    void creatShadowPassConstantBuffer();
    void updateShadowPassConstantBuffer();

private:
    void _createMainPassConstant(FMainPassConstant& constant);
    void _createShadowPassConstant(FShadowPassConstant& constant);

private:
    FRHI* mRHI;
    FRHIRenderTarget* mRenderTarget;
    FScene* mScene;
    FView* mView;
    FRHIConstantBuffer* mMainPassConstantBuffer;
    FRHIConstantBuffer* mShadowPassConstantBuffer;
    FRHIRenderTarget* mShadowMap;
    FMatrix4x4 mShadowTransform;
};
