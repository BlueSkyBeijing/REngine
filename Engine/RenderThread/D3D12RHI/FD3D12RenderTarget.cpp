#include "PrecompiledHeader.h"
#include "FD3D12RenderTarget.h"
#include "FInputManager.h"
#include "FD3D12RHI.h"
#include "WindowsUtility.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRenderThread.h"

#include <Windows.h>
#include <stdexcept>
#include <functional>
#include <memory>

FD3D12RenderTarget::FD3D12RenderTarget(
    unsigned int width,
    unsigned int height,
    DXGI_FORMAT format):
    FRHIRenderTarget(width, height)
{
}

FD3D12RenderTarget::~FD3D12RenderTarget()
{
}

void FD3D12RenderTarget::Init()
{

}

void FD3D12RenderTarget::UnInit()
{

}

FD3D12RenderWindow::FD3D12RenderWindow(
    unsigned int width, 
    unsigned int height,
    DXGI_FORMAT format):
    FRHIRenderWindow(width, height),
    mDXGISwapChain(nullptr),
    mSwapChainBufferCount(SWAPCHAIN_BUFFER_COUNT),
    mChainBufferndex(0),
    mRTVDescriptorSize(0),
    mDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
    mDSVDescriptorSize(0),
    mBackBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
{
}

FD3D12RenderWindow::~FD3D12RenderWindow()
{
}

void FD3D12RenderWindow::Init()
{
}

void FD3D12RenderWindow::UnInit()
{
}

void FD3D12RenderWindow::Present()
{
    FRHI* rhi = TSingleton<FEngine>::GetInstance().GetRenderThread()->GetRHI();

    rhi->Present();
}

Microsoft::WRL::ComPtr <ID3D12Resource> FD3D12RenderWindow::GetRenderBuffer() const
{
    return mRenderTargets[mChainBufferndex];
}

D3D12_CPU_DESCRIPTOR_HANDLE FD3D12RenderWindow::GetRenderBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(),
        mChainBufferndex,
        mRTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE FD3D12RenderWindow::GetDepthStencilView() const
{
    return mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart();
}