#pragma once
#include "FBuffer.h"

class FDeviceManager;
class FRenderWindow;
class FSceneManager;
class FScene;
class FView;
class FRenderTarget;
class FShaderManager;
class FRootSignatureManager;
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
	FRenderer(FRenderWindow* renderWindow, FScene* scene, FView* view);
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
    virtual void setRenderTarget(FRenderTarget* renderTarget);
    virtual void setViewPort();
    virtual void drawRenderables();
    virtual void postProcess();
    virtual void postRender();

    void createPassConstantBuffer();

private:
    FDevice* mDevice;
    FRenderWindow* mRenderWindow;
    FScene* mScene;
    FView* mView;
    FConstantBuffer<FPassConstant>* mPassConstantBuffer;
};
