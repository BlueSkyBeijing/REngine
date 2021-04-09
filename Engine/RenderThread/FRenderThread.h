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

    void InitView(FVector3& position, FVector3& target, FVector3& up);

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

    inline int32 GetFrameNum() const
    {
        return mSyncNum;
    }
protected:
    virtual void start();

    virtual void init();
    virtual void unInit();

    virtual void update();

    virtual void loop();

    virtual void processRenderCommand();

    virtual void waitResourceReady();

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
    int32 mSyncNum;

    std::thread* mRenderThread;
    std::vector<FRenderCommand*> mRenderCommands;
};
