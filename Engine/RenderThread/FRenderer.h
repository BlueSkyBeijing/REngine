#pragma once
#include "Prerequisite.h"

#include "FRHIBuffer.h"

struct FPassConstant
{
    FMatrix4x4 View;
    FMatrix4x4 Proj;
    FMatrix4x4 ViewProj;
    FVector3 CameraPos;
    float a;
    FVector3 CameraDir;
    float b;
    FVector3 DirectionalLightDir;
    float c;
    FVector3 DirectionalLightColor;
    float d;
};

class FRenderer
{
public:
    FRenderer(FRHIRenderWindow* renderWindow, FScene* scene, FView* view);
    ~FRenderer();

    virtual void Init();
    virtual void UnInit();

    virtual void CreateRenderResources();

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

    void createPassConstantBuffer();

private:
    FRHI* mRHI;
    FRHIRenderWindow* mRenderWindow;
    FScene* mScene;
    FView* mView;
    FRHIConstantBuffer* mPassConstantBuffer;
};
