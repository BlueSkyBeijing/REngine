#pragma once
#include "Prerequisite.h"

#include "FConfigManager.h"

class FRenderThread
{
public:
    FRenderThread(FEngine* engine);
    ~FRenderThread();

    void Start();

    void Exit();

    void AddToScene(FRenderProxy* renderProxy);

    void SetView(FView* view);

    void SetDirectionalLight(FDirectionalLight* light);

    void OnProduceOneFrame();

    void OnWindowResize(int32 newWidth, int32 newHeight);

    void EnqueueRenderCommand(FRenderCommand* renderCommand);

    inline void MarkLoadCompleted()
    {
        mLoadCompleted = true;
    }

    inline bool IsInited() const
    {
        return mInited;
    }

    inline FRHI* GetRHI() const
    {
        return mRHI;
    }

    inline int32 GetProcessFrameNum() const
    {
        return mFrameSyncSignal;
    }
protected:
    void start();

    void init();
    void unInit();

    void update();

    void loop();

    void processRenderCommand();
    void processRemainRenderCommand();

    void syncMainThread();

private:
    bool mHeartbeat;
    std::atomic_bool mLoadCompleted;
    std::atomic_bool mInited;
    int32 mFrameCount;

    FEngine* mEngine;
    FRHI* mRHI;
    FRHIRenderWindow* mRenderWindow;
    FRenderer* mRenderer;
    FScene* mScene;
    FView* mView;

    std::mutex mRenderMutex;
    std::condition_variable mRenderCondition;
    std::atomic_int32_t mFrameSyncSignal;

    std::thread* mRenderThread;
    std::vector<FRenderCommand*> mRenderCommands[FRAME_BUFFER_NUM];
};
