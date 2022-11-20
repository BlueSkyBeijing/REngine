#include "PrecompiledHeader.h"

#include "FInputManager.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "UWorld.h"
#include "UCamera.h"
#include "FRenderThread.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FView.h"
#include "FRenderCommand.h"
#include "FPlayerController.h"
#include "FConsoleVariableManager.h"

FInputManager::FInputManager():
    mDeltaScale(1000.0f),
    mPan(false)
{
}

FInputManager::~FInputManager()
{
}

void FInputManager::Init()
{
    mKeyDown = false;
    mTriggerMove = false;
    mEject = true;
}

void FInputManager::UnInit()
{
}

void FInputManager::Update(float deltaSeconds)
{
    OnKeyInput(deltaSeconds);
}

void FInputManager::ProcessQuit()
{
    TSingleton<FEngine>::GetInstance().Exit();
}

LRESULT CALLBACK FInputManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    FInputManager* inputManager = reinterpret_cast<FInputManager*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
    {
        LPCREATESTRUCT pCreateStruct{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        inputManager->ProcessQuit();
        break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        inputManager->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        inputManager->OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEMOVE:
        inputManager->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEWHEEL:
        inputManager->OnMouseWheel(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_SIZE:
        inputManager->OnResize(lParam);
        break;
    case WM_KEYDOWN:
        inputManager->OnKeyDown(lParam);
        break;
    case WM_KEYUP:
        inputManager->OnKeyUp(lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void FInputManager::OnKeyDown(WPARAM btnState)
{
    mKeyDown = true;
}

void FInputManager::OnKeyUp(WPARAM btnState)
{
    mKeyDown = false;
}

void FInputManager::OnMouseDown(WPARAM btnState, int32 x, int32 y)
{
    mPan = btnState & MK_MBUTTON;

    mLastMousePos.x() = x;
    mLastMousePos.y() = y;
    SetCapture(TSingleton<FEngine>::GetInstance().GetWindowHandle());
}

void FInputManager::OnMouseUp(WPARAM btnState, int32 x, int32 y)
{
    ReleaseCapture();
}

void FInputManager::OnMouseMove(WPARAM btnState, int32 x, int32 y)
{
    FEngine& engine = TSingleton<FEngine>::GetInstance();

    if ((btnState & MK_LBUTTON) || (btnState & MK_MBUTTON) || (btnState & MK_RBUTTON))
    {
        const float deltaScale = 0.0025f;
        float dx = -(deltaScale * static_cast<float>(x - mLastMousePos.x()));
        float dy = -(deltaScale * static_cast<float>(y - mLastMousePos.y()));

        if (mEject)
        {
            if (mPan)
            {
                const float panScale = 0.5f * mDeltaScale;
                engine.GetWorld()->GetCamera()->Pan(dx * panScale, dy * panScale);
            }
            else
            {
                if (btnState & MK_RBUTTON)
                {
                    engine.GetWorld()->GetCamera()->AdjustPitch(-dy);
                    engine.GetWorld()->GetCamera()->AdjustYaw(-dx);
                }
                else
                {
                    engine.GetWorld()->GetCamera()->AdjustMoveStraight(dy * 1000.0f);
                    engine.GetWorld()->GetCamera()->AdjustYaw(-dx);
                }
            }
        }
        else
        {
            TSingleton<FPlayerController>::GetInstance().AdjustPitch(dy);
            TSingleton<FPlayerController>::GetInstance().Turn(dx);
        }

    }

    mLastMousePos.x() = x;
    mLastMousePos.y() = y;
}

void FInputManager::OnMouseWheel(WPARAM btnState, int32 x, int32 y)
{
    int loVal = LOWORD(btnState);
    short hiVal = HIWORD(btnState);
    if (mKeyDown)
    {
        mDeltaScale += hiVal * 0.1f;
        mDeltaScale = std::max(0.1f, mDeltaScale);
    }

    if (mEject)
    {
        const float deltaScale = mDeltaScale;

        FEngine& engine = TSingleton<FEngine>::GetInstance();
        engine.GetWorld()->GetCamera()->AdjustMoveStraight(0.0005f * mDeltaScale * hiVal);
    }

}

void FInputManager::OnKeyInput(float deltaSeconds)
{
    FEngine& engine = TSingleton<FEngine>::GetInstance();
    const float deltaScale = mDeltaScale;
    const float deltaScalePlayerMove = 300.0f;
    const float deltaScaleTurn = 2.0f;

    if ((GetAsyncKeyState('E') & 0x8000))
    {
        mEject = !mEject;
    }

    if ((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000))
    {
        if (mEject)
        {
            engine.GetWorld()->GetCamera()->AdjustMoveStraight(deltaSeconds * deltaScale);
        }
        else
        {
            TSingleton<FPlayerController>::GetInstance().MoveStraight(deltaSeconds * deltaScalePlayerMove);
        }
    }

    if ((GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000))
    {
        if (mEject)
        {
            engine.GetWorld()->GetCamera()->AdjustMoveStraight(-deltaSeconds * deltaScale);
        }
        else
        {
            TSingleton<FPlayerController>::GetInstance().MoveStraight(-deltaSeconds * deltaScalePlayerMove);
        }
    }

    if ((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000))
    {
        if (!mTriggerMove)
        {
            TSingleton<FPlayerController>::GetInstance().EnterMoveState();

            mTriggerMove = true;
        }
    }
    else
    {
        if (mTriggerMove)
        {
            TSingleton<FPlayerController>::GetInstance().EnterStandState();

            mTriggerMove = false;
        }
    }

    if ((GetAsyncKeyState('A') & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000))
    {
        if (mEject)
        {
            engine.GetWorld()->GetCamera()->AdjustYaw(-deltaSeconds);
        }
        else
        {
            TSingleton<FPlayerController>::GetInstance().Turn(deltaSeconds * deltaScaleTurn);
        }
    }

    if ((GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000))
    {
        if (mEject)
        {
            engine.GetWorld()->GetCamera()->AdjustYaw(deltaSeconds);
        }
        else
        {
            TSingleton<FPlayerController>::GetInstance().Turn(-deltaSeconds * deltaScaleTurn);
        }
    }

    //if ((GetAsyncKeyState(VK_OEM_3) & 0x8000))
    //{
    //    TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand();
    //}

}

void FInputManager::OnResize(LPARAM lParam)
{
    int32 width = LOWORD(lParam);
    int32 height = HIWORD(lParam);

    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();

    if (renderThread != nullptr)
    {
        ENQUEUE_RENDER_COMMAND([renderThread, width, height]
        {
            renderThread->OnWindowResize(width, height);
        });
    }
}
