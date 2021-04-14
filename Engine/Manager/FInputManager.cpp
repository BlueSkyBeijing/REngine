#include "PrecompiledHeader.h"

#include "FInputManager.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "UWorld.h"
#include "UCamera.h"
#include <WindowsX.h>

FInputManager::FInputManager()
{
}

FInputManager::~FInputManager()
{
}

void FInputManager::Init()
{
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
    case WM_QUIT:
        inputManager->ProcessQuit();
        break;
    case WM_LBUTTONDOWN:
        inputManager->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONUP:
        inputManager->OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEMOVE:
        inputManager->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void FInputManager::OnMouseDown(WPARAM btnState, int32 x, int32 y)
{
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
    if ((btnState & MK_LBUTTON) != 0)
    {
        const float deltaScale = 0.0025f;
        float dx = (deltaScale * static_cast<float>(x - mLastMousePos.x()));
        float dy = (deltaScale * static_cast<float>(y - mLastMousePos.y()));

        engine.GetWorld()->GetCamera()->AdjustPitch(dy);
        engine.GetWorld()->GetCamera()->AdjustYaw(dx);
    }

    mLastMousePos.x() = x;
    mLastMousePos.y() = y;
}

void FInputManager::OnKeyInput(float deltaSeconds)
{
    FEngine& engine = TSingleton<FEngine>::GetInstance();
    const float deltaScale = 1000.0f;

    if ((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000))
    {
        engine.GetWorld()->GetCamera()->AdjustMoveStraight(deltaSeconds * deltaScale);
    }

    if ((GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000))
    {
        engine.GetWorld()->GetCamera()->AdjustMoveStraight(-deltaSeconds * deltaScale);
    }

    if (GetAsyncKeyState('A') & 0x8000)
    {
        engine.GetWorld()->GetCamera()->AdjustMoveLaterally(-deltaSeconds * deltaScale);
    }

    if (GetAsyncKeyState('D') & 0x8000)
    {
        engine.GetWorld()->GetCamera()->AdjustMoveLaterally(deltaSeconds * deltaScale);
    }

    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        engine.GetWorld()->GetCamera()->AdjustYaw(-deltaSeconds);
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        engine.GetWorld()->GetCamera()->AdjustYaw(deltaSeconds);
    }
}
