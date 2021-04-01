#include "FRenderTarget.h"
#include "FInputManager.h"
#include "FDeviceManager.h"
#include "FDevice.h"
#include "WindowsUtility.h"

#include <Windows.h>
#include <stdexcept>
#include <functional>
#include <memory>

FRenderTarget::FRenderTarget(
    unsigned int width,
    unsigned int height,
    DXGI_FORMAT format):
    Width(width),
    Height(height),
    Format(format)
{
}

FRenderTarget::~FRenderTarget()
{
}

void FRenderTarget::Init()
{

}

void FRenderTarget::UnInit()
{

}

FRenderWindow::FRenderWindow(
    unsigned int width, 
    unsigned int height,
    DXGI_FORMAT format):
    FRenderTarget(width, height, format),
    mWindowHandle(nullptr),
    mDXGISwapChain(nullptr),
    mSwapChainBufferCount(SWAPCHAIN_BUFFER_COUNT),
    mChainBufferndex(0),
    mRTVDescriptorSize(0),
    mDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
    mDSVDescriptorSize(0),
    mBackBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
{
}

FRenderWindow::~FRenderWindow()
{
}

void FRenderWindow::Init()
{
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = FInputManager::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = (HMODULE)GetModuleHandle(0);
    wcex.hIcon = LoadIcon(NULL, IDI_SHIELD);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"WindowClass";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (RegisterClassEx(&wcex) == 0)
    {
        //print error
        return;
    }

    RECT rect{ 0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height) };
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

    mWindowHandle = CreateWindowEx(0, L"WindowClass", L"RenderWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, nullptr, &TSingleton<FInputManager>::GetInstance());
    if (!mWindowHandle)
    {
        //print error
        return;
    }

    ShowWindow(mWindowHandle, SW_SHOW);
    UpdateWindow(mWindowHandle);

    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->CreateRenderTarget(this);
}

void FRenderWindow::UnInit()
{
    DestroyWindow(mWindowHandle);
}

void FRenderWindow::Present()
{
    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->Present();
}

Microsoft::WRL::ComPtr <ID3D12Resource> FRenderWindow::GetRenderBuffer() const
{
    return mRenderTargets[mChainBufferndex];
}

D3D12_CPU_DESCRIPTOR_HANDLE FRenderWindow::GetRenderBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(),
        mChainBufferndex,
        mRTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE FRenderWindow::GetDepthStencilView() const
{
    return mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart();
}