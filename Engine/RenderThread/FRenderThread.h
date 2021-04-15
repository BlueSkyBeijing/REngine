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

    void SetView(FVector3& position, FVector3& target, FVector3& up, FVector3& right, FVector3& look, float fov, float aspectRatio);

    void OnReadyToRender();

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

private:
    bool mHeartbeat;
    std::atomic_bool mLoadCompleted;
    std::atomic_bool mInited;

    FEngine* mEngine;
    FRHI* mRHI;
    FRHIRenderWindow* mRenderWindow;
    FRenderer* mRenderer;
    FScene* mScene;
    FView* mView;

    std::mutex mRenderMutex;
    std::condition_variable mRenderCondition;
    std::atomic_int32_t mProcessFrameNum;

    std::thread* mRenderThread;
    std::vector<FRenderCommand*> mRenderCommands[FRAME_BUFFER_NUM];
};
