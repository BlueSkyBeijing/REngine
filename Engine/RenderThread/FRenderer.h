#pragma once
#include "Prerequisite.h"

#include "FRHIBuffer.h"

struct FPassConstant
{
    FMatrix4x4 View;
    FMatrix4x4 Proj;
    FMatrix4x4 ViewProj;
    FVector3 CameraPos;
    float _Placeholder0;
    FVector3 CameraDir;
    float _Placeholder1;
    FVector3 DirectionalLightDir;
    float _Placeholder2;
    FVector3 DirectionalLightColor;
    float _Placeholder3;
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

    void createPassConstantBuffer();
    void updatePassConstantBuffer();

private:
    void _createPassConstant(FPassConstant& constant);

private:
    FRHI* mRHI;
    FRHIRenderWindow* mRenderWindow;
    FScene* mScene;
    FView* mView;
    FRHIConstantBuffer* mPassConstantBuffer;
    FRHIRenderTarget* mShadowMap;
};
