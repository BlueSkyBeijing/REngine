#include "PrecompiledHeader.h"
#include "FInputManager.h"
#include "FEngine.h"
#include "TSingleton.h"

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

        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}