#include "PrecompiledHeader.h"

#include "FRenderThread.h"
#include "FRHI.h"
#include "FRHICommandList.h"
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
    mInited(false)
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

void FRenderThread::SetView(FVector3& position, FVector3& target, FVector3& up)
{
    mView->Position = position;
    mView->Target = target;
    mView->Up = up;
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
    mRenderer->Render();

    mRenderWindow->Present();
}

void FRenderThread::loop()
{
    waitResourceReady();
    processRenderCommand();

    while (mHeartbeat)
    {
        update();
    };
}

void FRenderThread::processRenderCommand()
{
    for (auto it = mRenderCommands.begin(); it != mRenderCommands.end(); it++)
    {
        FRenderCommand* command = *it;
        command->Excecute();
    }

    mRenderCommands.clear();
}

void FRenderThread::waitResourceReady()
{
    //@todo; need to improve implement way
    //load completed 
    while (!mLoadCompleted)
    {
        Sleep(100);
        //wait
    }

    //create render resources
    mRenderer->CreateRenderResources();
}
