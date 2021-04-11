#include "PrecompiledHeader.h"

#include "FEngine.h"
#include "FRenderThread.h"
#include "FInputManager.h"
#include "FLogManager.h"
#include "FConfigManager.h"
#include "UWorld.h"
#include "UCamera.h"


FEngine::FEngine() :mHeartbeat(true),
mInited(false),
mRenderThread(nullptr),
mWorld(nullptr),
mWindowWidth(1024),
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
    TSingleton<FInputManager>::GetInstance().Init();

    createWindow();

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

    TSingleton<FInputManager>::GetInstance().UnInit();

    mInited = false;
}

void FEngine::loop()
{
    while (mHeartbeat)
    {
        //@todo; need to improve implement way
        MSG msg;
        if (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        update();
    }
}

void FEngine::Exit()
{
    mHeartbeat = false;
}

void FEngine::update()
{
    syncRenderThread();

    mCurFrameTime = mTimer.now();
    mDeltaSeconds = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(mCurFrameTime - mLastFrameTime).count() / 1000.0f;
    mLastFrameTime = mCurFrameTime;

    mRenderThread->SetView(mWorld->GetCamera()->Position, mWorld->GetCamera()->Target, mWorld->GetCamera()->Up);

    mRenderThread->OnNewFrame();
}

void FEngine::createWindow()
{
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

void FEngine::syncRenderThread()
{
    while (mRenderThread->GetProcessFrameNum() >= FRAME_BUFFER_NUM)
    {
        Sleep(1);
    }
}
