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

    void OnMouseDown(WPARAM btnState, int32 x, int32 y);
    void OnMouseUp(WPARAM btnState, int32 x, int32 y);
    void OnMouseMove(WPARAM btnState, int32 x, int32 y);
    void OnKeyInput(float deltaTime);

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    FVector2i mLastMousePos;
};
