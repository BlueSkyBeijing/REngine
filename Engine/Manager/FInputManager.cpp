﻿#include "PrecompiledHeader.h"

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
    case WM_KEYDOWN:
        //inputManager->OnKeyInput();
        break;
        //case WM_LBUTTONDOWN:
        //    inputManager->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        //    break;
        //case WM_LBUTTONUP:
        //    inputManager->OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEMOVE:
        inputManager->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void FInputManager::OnMouseDown(WPARAM btnState, int x, int y)
{
    mLastMousePos.x() = x;
    mLastMousePos.y() = y;
    //SetCapture(mhMainWnd);

}

void FInputManager::OnMouseUp(WPARAM btnState, int x, int y)
{
    ReleaseCapture();
}

void FInputManager::OnMouseMove(WPARAM btnState, int x, int y)
{
    FEngine& engine = TSingleton<FEngine>::GetInstance();
    if ((btnState & MK_LBUTTON) != 0)
    {
        // Make each pixel correspond to a quarter of a degree.
        float dx = (0.0025f * static_cast<float>(x - mLastMousePos.x()));
        float dy = (0.0025f * static_cast<float>(y - mLastMousePos.y()));

        engine.GetWorld()->GetCamera()->SetPitch(dy);
        engine.GetWorld()->GetCamera()->SetYaw(dx);
    }

    mLastMousePos.x() = x;
    mLastMousePos.y() = y;
}

void FInputManager::OnKeyInput()
{
    FEngine& engine = TSingleton<FEngine>::GetInstance();
    const float deltaSeconds = engine.GetDeltaSeconds();
    const float deltaScale = 1000.0f;

    if ((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000))
    {
        FVector3 direction = engine.GetWorld()->GetCamera()->Target - engine.GetWorld()->GetCamera()->Position;
        direction = direction.normalized();
        engine.GetWorld()->GetCamera()->Position += deltaSeconds * direction * deltaScale;
    }

    if ((GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000))
    {
        FVector3 direction = engine.GetWorld()->GetCamera()->Target - engine.GetWorld()->GetCamera()->Position;
        direction = direction.normalized();
        engine.GetWorld()->GetCamera()->Position -= deltaSeconds * direction * deltaScale;
    }

    if (GetAsyncKeyState('A') & 0x8000)
    {
        engine.GetWorld()->GetCamera()->Position -= deltaSeconds * engine.GetWorld()->GetCamera()->Right * deltaScale;
    }

    if (GetAsyncKeyState('D') & 0x8000)
    {
        engine.GetWorld()->GetCamera()->Position += deltaSeconds * engine.GetWorld()->GetCamera()->Right * deltaScale;
    }

    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        engine.GetWorld()->GetCamera()->SetYaw(-deltaSeconds);
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        engine.GetWorld()->GetCamera()->SetYaw(deltaSeconds);
    }


}
