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
    mDeltaScale(200.0f),
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
        inputManager->OnResize(wParam);
        break;
    case WM_KEYDOWN:
        inputManager->OnKeyDown(wParam);
        break;
    case WM_KEYUP:
        inputManager->OnKeyUp(wParam);
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

    int loVal = LOWORD(btnState);

    switch (btnState)
    {
        case 0x30:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum0Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        case 0x31:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum1Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        case 0x32:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum2Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        case 0x33:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum3Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        case 0x34:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum4Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        case 0x35:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum5Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        case 0x36:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum6Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        case 0x37:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum7Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        case 0x38:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum8Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        case 0x39:
        {
            std::string cmd = TSingleton<FConfigManager>::GetInstance().KeyNum9Cmd;
            TSingleton<FConsoleVariableManager>::GetInstance().ProcessCommand(cmd);
        }
        break;
        default:
        break;
    }
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
            if (UCamera* cam = engine.GetWorld()->GetCamera())
            {
                if (mPan)
                {
                    const float panScale = 0.05f * mDeltaScale;
                    cam->Pan(dx * panScale, dy * panScale);
                }
                else
                {
                    if (btnState & MK_RBUTTON)
                    {
                        cam->AdjustPitch(-dy);
                        cam->AdjustYaw(-dx);
                    }
                    else
                    {
                        cam->AdjustMoveStraight(dy * 1000.0f);
                        cam->AdjustYaw(-dx);
                    }
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
        if (UCamera* cam = engine.GetWorld()->GetCamera())
        {
            cam->AdjustMoveStraight(0.0001f * mDeltaScale * hiVal);
        }
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
