#pragma once
#include "TSingleton.h"

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

protected:
    virtual void init();
    virtual void unInit();

    virtual void loop();
    virtual void update();

    void startRenderThread();
    void stopRenderingThread();

	void waitRenderThreadInited();
	void waitRenderThreadUninited();

private:
    bool mHeartbeat;
    bool mInited;

    FRenderThread* mRenderThread;
    UWorld* mWorld;
};
