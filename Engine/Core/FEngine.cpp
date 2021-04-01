#include "FEngine.h"
#include "FRenderThread.h"
#include "FInputManager.h"
#include "FLogManager.h"
#include "FConfigManager.h"
#include "UWorld.h"

#include <thread>
#include <mutex>
#include <condition_variable>

FEngine::FEngine():mHeartbeat(true),
    mInited(false),
    mRenderThread(nullptr),
    mWorld(nullptr)
{
}

FEngine::~FEngine()
{
}

void FEngine::Launch()
{
    init();
    loop();
    unInit();
}

void FEngine::init()
{
    //start render thread
    startRenderThread();

    //load map
    mWorld = new UWorld(this);
    mWorld->Load();

    mInited = true;
}

void FEngine::unInit()
{
    if (!mInited)
    {
        return;
    }

    // stop render thread
    stopRenderingThread();

    // unload map
    mWorld->Unload();
    delete mWorld;
    mWorld = nullptr;

    mInited = false;
}

void FEngine::loop()
{
    while(mHeartbeat)
    {
        update();
    }
}

void FEngine::Exit()
{
    mHeartbeat = false;
}

void FEngine::update()
{
}

void FEngine::startRenderThread()
{
    mRenderThread = new FRenderThread(this);

    mRenderThread->Start();

    waitRenderThreadInited();
}

void FEngine::stopRenderingThread()
{
    mRenderThread->Exit();

    waitRenderThreadUninited();

    delete mRenderThread;
    mRenderThread = nullptr;
}

void FEngine::waitRenderThreadInited()
{
	//@todo; need to improve implement way
	while (!mRenderThread->IsInited())
	{
		Sleep(100);
	}
}

void FEngine::waitRenderThreadUninited()
{
	//@todo; need to improve implement way
	//wait until uninted
	while (mRenderThread->IsInited())
	{
		Sleep(100);
	}
}