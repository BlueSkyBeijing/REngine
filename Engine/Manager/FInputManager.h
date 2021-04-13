#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FRenderThread;

class FInputManager : TSingleton<FInputManager>
{
public:
    FInputManager();
    ~FInputManager();

    void Init();
    void UnInit();

    void Update(float deltaSeconds);

    void ProcessQuit();

    void OnMouseDown(WPARAM btnState, int x, int y);
    void OnMouseUp(WPARAM btnState, int x, int y);
    void OnMouseMove(WPARAM btnState, int x, int y);
    void OnKeyInput(float deltaTime);

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    FRenderThread* mRenderThread;
    FVector2i mLastMousePos;
};
