#include "PrecompiledHeader.h"

#include "FD3D12RenderTarget.h"
#include "FInputManager.h"
#include "FD3D12RHI.h"
#include "WindowsUtility.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRenderThread.h"

FD3D12RenderTarget::FD3D12RenderTarget(
    uint32 width,
    uint32 height,
    uint32 numTarget) :
    FRHIRenderTarget(width, height, numTarget)
{
}

FD3D12RenderTarget::~FD3D12RenderTarget()
{
}

void FD3D12RenderTarget::Init()
{
    FRHIRenderTarget::Init();
}

void FD3D12RenderTarget::UnInit()
{
    FRHIRenderTarget::UnInit();

}

FD3D12RenderWindow::FD3D12RenderWindow(
    unsigned int width,
    unsigned int height,
    DXGI_FORMAT format, uint32 numTarget) :
    FRHIRenderWindow(width, height, numTarget),
    mDXGISwapChain(nullptr)
{
}

FD3D12RenderWindow::~FD3D12RenderWindow()
{
}

void FD3D12RenderWindow::Init()
{
    FRHIRenderWindow::Init();
}

void FD3D12RenderWindow::UnInit()
{
    FRHIRenderWindow::UnInit();
}

void FD3D12RenderWindow::Present()
{
    THROW_IF_FAILED(mDXGISwapChain->Present(0, 0));

    mRenderTargetIndex = (mRenderTargetIndex + 1) % NumTarget;
}
