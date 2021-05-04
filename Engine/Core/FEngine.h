#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"


class FEngine
{
    friend class TSingleton<FEngine>;
public:
    virtual void Launch();
    virtual void Exit();

    inline FRenderThread* GetRenderThread() const
    {
        return mRenderThread;
    }

    inline UWorld* GetWorld() const
    {
        return mWorld;
    }

    inline float GetDeltaSeconds()
    {
        return mDeltaSeconds;
    }

    inline HWND GetWindowHandle() const
    {
        return mWindowHandle;
    }

    inline uint32 GetWindowWidth() const
    {
        return mWindowWidth;
    }

    inline uint32 GetWindowHeight() const
    {
        return mWindowHeight;
    }

protected:
    virtual void init();
    virtual void unInit();

    virtual void loop();
    virtual void update();

    void createWindow();

    void startRenderThread();
    void stopRenderingThread();

    void waitRenderThreadInited();
    void waitRenderThreadUninited();

    void syncRenderThread();

private:
    FEngine();
    virtual ~FEngine();

private:
    bool mHeartbeat;
    bool mInited;

    FRenderThread* mRenderThread;
    UWorld* mWorld;

    HWND mWindowHandle;
    uint32 mWindowWidth;
    uint32 mWindowHeight;

    std::chrono::high_resolution_clock mTimer;
    std::chrono::steady_clock::time_point mLastFrameTime;
    std::chrono::steady_clock::time_point mCurFrameTime;
    float mDeltaSeconds;
};
