#pragma once
#include "TSingleton.h"

#include <Windows.h>

class FRenderThread;

class FInputManager : TSingleton<FInputManager>
{
public:
    FInputManager();
	~FInputManager();

    void Init();
    void UnInit();

	void ProcessQuit();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	FRenderThread* mRenderThread;
};
