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
    mProcessFrameNum(0)
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
}

void FRenderThread::AddToScene(FRenderProxy* renderProxy)
{
    mScene->AddRenderable(renderProxy);
}

void FRenderThread::SetView(FVector3& position, FVector3& target, FVector3& up, FVector3& right, FVector3& look, float fov, float aspectRatio)
{
    mView->Position = position;
    mView->Target = target;
    mView->Up = up;
    mView->Right = right;
    mView->Look = look;
    mView->FOV = fov;
    mView->AspectRatio = aspectRatio;
}

void FRenderThread::OnReadyToRender()
{
    std::unique_lock<std::mutex> RenderLock(mRenderMutex);
    mProcessFrameNum++;
    mRenderCondition.notify_one();
}

void FRenderThread::EnqueueRenderCommand(FRenderCommand* renderCommand)
{
    const int32 frameIndex = mProcessFrameNum > 1 ? mProcessFrameNum.load() - 1 : mProcessFrameNum.load();

    mRenderCommands[frameIndex].push_back(renderCommand);
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
    const int32 frameIndex = mProcessFrameNum - 1;

    for (auto it = mRenderCommands[frameIndex].begin(); it != mRenderCommands[frameIndex].end(); it++)
    {
        FRenderCommand* command = *it;
        command->Excecute();

        delete command;
    }

    mRenderCommands[frameIndex].clear();
}

void FRenderThread::syncMainThread()
{
    std::unique_lock<std::mutex> RenderLock(mRenderMutex);
    if (mProcessFrameNum.load() > 0)
    {
        mProcessFrameNum--;
    }
    mRenderCondition.wait(RenderLock);
}
