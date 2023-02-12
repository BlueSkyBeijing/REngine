#include "PrecompiledHeader.h"

#include "FEngine.h"
#include "FRenderThread.h"
#include "FInputManager.h"
#include "FLogManager.h"
#include "FConfigManager.h"
#include "FModuleManager.h"
#include "FPlayerController.h"
#include "FResourceManager.h"
#include "FConsoleVariableManager.h"
#include "UWorld.h"
#include "UCamera.h"

//disable chinese typewriting
#include <imm.h>  
#pragma comment (lib ,"imm32.lib") 


FEngine::FEngine() :
    mHeartbeat(true),
    mInited(false),
    mRenderThread(nullptr),
    mWorld(nullptr),
    mWindowWidth(1366),
    mWindowHeight(768)
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
    TSingleton<FConfigManager>::GetInstance().Init();
    TSingleton<FLogManager>::GetInstance().Init();
    TSingleton<FModuleManager>::GetInstance().Init();   
    TSingleton<FInputManager>::GetInstance().Init();
    TSingleton<FPlayerController>::GetInstance().Init();
    TSingleton<FResourceManager>::GetInstance().Init();
    TSingleton<FConsoleVariableManager>::GetInstance().Init();

    TSingleton<FModuleManager>::GetInstance().LoadProjectMoudules(FConfigManager::ProjectDir, FConfigManager::ProjectName);

    createWindow();

    //start render thread
    startRenderThread();

    //load map
    mWorld = new UWorld(this);
    mWorld->Load();

    mInited = true;

    TSingleton<FLogManager>::GetInstance().LogMessage(LL_Info, "Engine init end.");
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

    TSingleton<FConfigManager>::GetInstance().UnInit();
    TSingleton<FInputManager>::GetInstance().UnInit();
    TSingleton<FLogManager>::GetInstance().UnInit();
    TSingleton<FModuleManager>::GetInstance().UnInit();
    TSingleton<FPlayerController>::GetInstance().UnInit();
    TSingleton<FResourceManager>::GetInstance().UnInit();
    TSingleton<FConsoleVariableManager>::GetInstance().UnInit();

    mInited = false;
}

void FEngine::loop()
{
    while (mHeartbeat)
    {
        //@todo; need to improve implement way
        MSG msg;
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                mHeartbeat = false;
            }
        }
        else
        {
            update();
        }
    }
}

void FEngine::Exit()
{
    mHeartbeat = false;
}

void FEngine::update()
{
    TSingleton<FInputManager>::GetInstance().Update(mDeltaSeconds);
    TSingleton<FPlayerController>::GetInstance().Update(mDeltaSeconds);
    syncRenderThread();

    mCurFrameTime = mTimer.now();
    mDeltaSeconds = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(mCurFrameTime - mLastFrameTime).count() / 1000.0f;
    mLastFrameTime = mCurFrameTime;

    mWorld->Update(mDeltaSeconds);

    mRenderThread->OnProduceOneFrame();
}

void FEngine::createWindow()
{
    mWindowWidth = TSingleton<FConfigManager>::GetInstance().WindowWidth;
    mWindowHeight = TSingleton<FConfigManager>::GetInstance().WindowHeight;

    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = FInputManager::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = (HMODULE)GetModuleHandle(0);
    wcex.hIcon = LoadIcon(NULL, IDI_SHIELD);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"WindowClass";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wcex) == 0)
    {
        //print error
        return;
    }

    RECT rect{ 0, 0, static_cast<LONG>(mWindowWidth), static_cast<LONG>(mWindowHeight) };
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

    mWindowHandle = CreateWindowEx(0, L"WindowClass", L"RenderWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, nullptr, &TSingleton<FInputManager>::GetInstance());
    if (!mWindowHandle)
    {
        //print error
        return;
    }

    //disable chinese typewriting
    ImmAssociateContext(mWindowHandle, NULL);

    ShowWindow(mWindowHandle, SW_SHOW);
    UpdateWindow(mWindowHandle);

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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void FEngine::waitRenderThreadUninited()
{
    //@todo; need to improve implement way
    //wait until uninted
    while (mRenderThread->IsInited())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void FEngine::syncRenderThread()
{
    while (mRenderThread->GetProcessFrameNum() >= FRAME_BUFFER_NUM)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
