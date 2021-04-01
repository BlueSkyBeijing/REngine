#pragma once

#include "Utility.h"
#include "FRHI.h"

#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include<thread>

class FRenderer;
class FRenderWindow;
class FScene;
class FView;
class FInputManager;
class FEngine;
class FRenderProxy;
class FShaderManager;
class FShaderBindingsManager;
class FPipelineStateManager;

class FRenderThread
{
public:
	FRenderThread(FEngine* engine);
	~FRenderThread();

    void Start();

    void Exit();

    void AddToScene(FRenderProxy* renderProxy);
 
    void InitView(FVector3& position, FVector3& target, FVector3& up);

    void MarkLoadCompleted()
    {
        mLoadCompleted = true;
    }

    bool IsInited() const
    {
        return mInited;
    }

protected:
    virtual void start();

    virtual void init();
    virtual void unInit();

    virtual void update();

    virtual void loop();

    virtual void waitResourceReady();

private:
    bool mHeartbeat;
    bool mLoadCompleted;
    bool mInited;

    FEngine* mEngine;
    FRHI* mRHI;
    FRenderWindow* mRenderWindow;
    FRenderer* mRenderer;
    FScene* mScene;
    FView* mView;

    std::thread* mRenderThread;
};
