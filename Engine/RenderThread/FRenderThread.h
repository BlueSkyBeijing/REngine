#pragma once
#include "Prerequisite.h"

#include "FRHICommandList.h"


class FRenderThread
{
public:
    FRenderThread(FEngine* engine);
    ~FRenderThread();

    void Start();

    void Exit();

    void AddToScene(FRenderProxy* renderProxy);

    void SetView(FVector3& position, FVector3& target, FVector3& up);

    void OnNewFrame();

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
        return mProcessFrameNum;
    }
protected:
    void start();

    void init();
    void unInit();

    void update();

    void loop();

    void processRenderCommand();

    void syncMainThread();

    void waitResourceReady();

private:
    bool mHeartbeat;
    bool mLoadCompleted;
    bool mInited;

    FEngine* mEngine;
    FRHI* mRHI;
    FRHIRenderWindow* mRenderWindow;
    FRenderer* mRenderer;
    FScene* mScene;
    FView* mView;

    std::mutex mRenderMutex;
    std::condition_variable mRenderCondition;
    int32 mProcessFrameNum;

    std::thread* mRenderThread;
    std::vector<FRenderCommand*> mRenderCommands;
};
