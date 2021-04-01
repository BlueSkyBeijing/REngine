#include "FRenderThread.h"
#include "FDeviceManager.h"
#include "FDevice.h"
#include "FRenderTarget.h"
#include "FRenderer.h"
#include "FScene.h"
#include "FView.h"
#include "FEngine.h"
#include "FInputManager.h"
#include "FRootSignatureManager.h"
#include "FPipelineStateManager.h"

#include<thread>

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

void FRenderThread::InitView(FVector3& position, FVector3& target, FVector3& up)
{
    mView->Position = position;
    mView->Target = target;
    mView->Up = up;
}

void FRenderThread::init()
{
    TSingleton<FDeviceManager>::GetInstance().Init();
    TSingleton<FPipelineStateManager>::GetInstance().Init();
    TSingleton<FRootSignatureManager>::GetInstance().Init();

    mRenderWindow = new FRenderWindow(1024, 768, DXGI_FORMAT_R8G8B8A8_UNORM);
    mRenderWindow->Init();

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

    TSingleton<FRootSignatureManager>::GetInstance().UnInit();
    TSingleton<FPipelineStateManager>::GetInstance().UnInit();
    TSingleton<FDeviceManager>::GetInstance().UnInit();

    delete mRenderThread;
    mRenderThread = nullptr;

    mInited = false;

}

void FRenderThread::update()
{
    mRenderer->RenderOneFrame();

    mRenderWindow->Present();
}

void FRenderThread::loop()
{
    waitResourceReady();

    while (mHeartbeat)
    {
        update();

        //@todo; need to improve implement way
        MSG msg;
        if (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    };
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

