#pragma once
#include "TSingleton.h"

class FRenderThread;

class FInputManager : TSingleton<FInputManager>
{
public:
    FInputManager();
    ~FInputManager();

    void Init();
    void UnInit();

    void ProcessQuit();

    void OnMouseDown(WPARAM btnState, int x, int y);
    void OnMouseUp(WPARAM btnState, int x, int y);
    void OnKeyInput();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    FRenderThread* mRenderThread;
};
