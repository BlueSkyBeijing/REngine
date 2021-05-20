#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FRenderThread;

class FInputManager
{
    friend class TSingleton<FInputManager>;

public:
    void Init();
    void UnInit();

    void Update(float deltaSeconds);

    void ProcessQuit();

    void OnKeyDown(WPARAM btnState);
    void OnKeyUp(WPARAM btnState);

    void OnMouseDown(WPARAM btnState, int32 x, int32 y);
    void OnMouseUp(WPARAM btnState, int32 x, int32 y);
    void OnMouseMove(WPARAM btnState, int32 x, int32 y);
    void OnKeyInput(float deltaTime);
    void OnResize(LPARAM lParam);

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    FInputManager();
    virtual ~FInputManager();

private:
    FVector2i mLastMousePos;
    bool mKeyDown;
    bool mTriggerMove;

};
