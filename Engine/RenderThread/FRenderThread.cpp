#include "PrecompiledHeader.h"

#include "FRenderThread.h"
#include "FRHI.h"
#include "FRenderCommand.h"
#include "FD3D12RHI.h"
#include "FRHIRenderTarget.h"
#include "FRenderer.h"
#include "FScene.h"
#include "FView.h"
#include "FEngine.h"
#include "FInputManager.h"
#include "FShaderBindingsManager.h"
#include "FPipelineStateManager.h"
#include "FLight.h"


FRenderThread::FRenderThread(FEngine* engine) :
    mEngine(engine),
    mHeartbeat(true),
    mRenderWindow(nullptr),
    mRenderer(nullptr),
    mView(nullptr),
    mLoadCompleted(false),
    mRenderThread(nullptr),
    mScene(nullptr),
    mInited(false),
    mFrameSyncSignal(0)
{
}

FRenderThread::~FRenderThread()
{
}

void FRenderThread::Start()
{
    mRenderThread = new std::thread(
        [this]() { return this->start(); });
    mRenderThread->detach();
}

void FRenderThread::start()
{
    init();
    loop();
    unInit();
}

void FRenderThread::Exit()
{
    mHeartbeat = false;
    mRenderCondition.notify_one();
}

void FRenderThread::AddToScene(FRenderProxy* renderProxy)
{
    mScene->AddRenderable(renderProxy);
}

void FRenderThread::SetView(FView* view)
{
    assert(view != nullptr);

    mView->Position = view->Position;
    mView->Target = view->Target;
    mView->Up = view->Up;
    mView->Right = view->Right;
    mView->Look = view->Look;
    mView->FOV = view->FOV;
    mView->AspectRatio = view->AspectRatio;

    delete view;
}

void FRenderThread::SetDirectionalLight(FDirectionalLight* light)
{
    mScene->SetDirectionalLight(light);
}

void FRenderThread::OnProduceOneFrame()
{
    std::unique_lock<std::mutex> RenderLock(mRenderMutex);
    mFrameSyncSignal++;
    mFrameCount++;
    mRenderCondition.notify_one();
}

void FRenderThread::OnWindowResize(int32 newWidth, int32 newHeight)
{

}

void FRenderThread::EnqueueRenderCommand(FRenderCommand* renderCommand)
{
    const int32 frameIndexMainThread = mFrameCount % FRAME_BUFFER_NUM;

    mRenderCommands[frameIndexMainThread].push_back(renderCommand);
}

void FRenderThread::init()
{
    mRHI = new FD3D12RHI;
    mRHI->Init();

    TSingleton<FPipelineStateManager>::GetInstance().Init();
    TSingleton<FShaderBindingsManager>::GetInstance().Init();

    mRenderWindow = mRHI->CreateRenderWindow(mEngine->GetWindowWidth(), mEngine->GetWindowHeight());

    mScene = new FScene();
    mScene->Init();

    mView = new FView();

    mRenderer = new FRenderer(mRenderWindow, mScene, mView);
    mRenderer->Init();

    mInited = true;
}

void FRenderThread::unInit()
{
    processRemainRenderCommand();

    mRenderer->UnInit();
    delete mRenderer;
    mRenderer = nullptr;

    mScene->UnInit();
    delete mScene;
    mScene = nullptr;

    delete mView;
    mView = nullptr;

    mRenderWindow->UnInit();
    delete mRenderWindow;
    mRenderWindow = nullptr;

    TSingleton<FShaderBindingsManager>::GetInstance().UnInit();
    TSingleton<FPipelineStateManager>::GetInstance().UnInit();

    mRHI->UnInit();
    delete mRHI;
    mRHI = nullptr;

    delete mRenderThread;
    mRenderThread = nullptr;

    mInited = false;

}

void FRenderThread::update()
{
    syncMainThread();

    processRenderCommand();

    mRenderer->Render();

    mRenderWindow->Present();
}

void FRenderThread::loop()
{
    while (mHeartbeat)
    {
        update();
    };
}

void FRenderThread::processRenderCommand()
{
    assert(mFrameCount > 0);
    const int32 renderThreadDelayFrameNum = FRAME_BUFFER_NUM - 1;
    const int32 frameIndexRenderThread = (mFrameCount - 1 - renderThreadDelayFrameNum) % FRAME_BUFFER_NUM;

    for (auto it = mRenderCommands[frameIndexRenderThread].begin(); it != mRenderCommands[frameIndexRenderThread].end(); it++)
    {
        FRenderCommand* command = *it;
        command->Excecute();

        delete command;
    }

    mRenderCommands[frameIndexRenderThread].clear();
}

void FRenderThread::processRemainRenderCommand()
{
    assert(mFrameCount > 0);
    for (int32 frameRemianIndex = 1; frameRemianIndex < FRAME_BUFFER_NUM; frameRemianIndex++)
    {
        const int32 renderThreadDelayFrameNum = FRAME_BUFFER_NUM - 1;
        const int32 frameIndexRenderThread = (mFrameCount - 1 - renderThreadDelayFrameNum + frameRemianIndex) % FRAME_BUFFER_NUM;

        for (auto it = mRenderCommands[frameIndexRenderThread].begin(); it != mRenderCommands[frameIndexRenderThread].end(); it++)
        {
            FRenderCommand* command = *it;
            command->Excecute();

            delete command;
        }

        mRenderCommands[frameIndexRenderThread].clear();
    }
}

void FRenderThread::syncMainThread()
{
    std::unique_lock<std::mutex> RenderLock(mRenderMutex);
    mFrameSyncSignal--;
    mRenderCondition.wait(RenderLock);
}
