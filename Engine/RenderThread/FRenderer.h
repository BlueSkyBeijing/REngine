#pragma once
#include "FRHIBuffer.h"

class FRHIRenderWindow;
class FSceneManager;
class FScene;
class FView;
class FRHIRenderTarget;
class FShaderManager;
class FShaderBindingsManager;
class FPipelineStateManager;

struct FPassConstant
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

    virtual void CreateRenderResources();

    virtual void RenderOneFrame();

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
