#pragma once
#include "TSingleton.h"
#include "Utility.h"

class FRenderThread;
class UWorld;

class FEngine : TSingleton<FEngine>
{
public:
    FEngine();
    ~FEngine();

    virtual void Launch();
    virtual void Exit();

    inline FRenderThread* GetRenderThread() const
    {
        return mRenderThread;
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

private:
    bool mHeartbeat;
    bool mInited;

    FRenderThread* mRenderThread;
    UWorld* mWorld;

    HWND mWindowHandle;
    uint32 mWindowWidth;
    uint32 mWindowHeight;
};
