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
    FRHIRenderTarget(width, height, numTarget),
    mDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
    mRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
    mRTVDescriptorSize(0),
    mDSVDescriptorSize(0),
    mRenderTargetIndex(0)
{
}

FD3D12RenderTarget::~FD3D12RenderTarget()
{
}

void FD3D12RenderTarget::Init()
{
    mRenderTargets.resize(NumTarget);
    mRenderTargets.shrink_to_fit();
}

void FD3D12RenderTarget::UnInit()
{

}

Microsoft::WRL::ComPtr <ID3D12Resource> FD3D12RenderTarget::GetRenderTargetBuffer(uint32 index) const
{
    return mRenderTargets[index];
}

Microsoft::WRL::ComPtr <ID3D12Resource> FD3D12RenderTarget::GetDepthStencilBuffer() const
{
    return mDepthStencilBuffer;
}

D3D12_CPU_DESCRIPTOR_HANDLE FD3D12RenderTarget::GetRenderTargetView(uint32 index) const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(),
        PosInHeap + index,
        mRTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE FD3D12RenderTarget::GetDepthStencilView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart(), PosInHeapDSV, mDSVDescriptorSize);
}

FD3D12RenderWindow::FD3D12RenderWindow(
    unsigned int width,
    unsigned int height,
    DXGI_FORMAT format, uint32 numTarget) :
    FRHIRenderWindow(width, height, numTarget),
    mDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
    mRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
    mRTVDescriptorSize(0),
    mDSVDescriptorSize(0),
    mRenderTargetIndex(0),
    mDXGISwapChain(nullptr)
{
}

FD3D12RenderWindow::~FD3D12RenderWindow()
{
}

void FD3D12RenderWindow::Init()
{
    mRenderTargets.resize(NumTarget);
    mRenderTargets.shrink_to_fit();
}

void FD3D12RenderWindow::UnInit()
{

}

Microsoft::WRL::ComPtr <ID3D12Resource> FD3D12RenderWindow::GetRenderTargetBuffer(uint32 index) const
{
    return mRenderTargets[index];
}

Microsoft::WRL::ComPtr <ID3D12Resource> FD3D12RenderWindow::GetDepthStencilBuffer() const
{
    return mDepthStencilBuffer;
}

D3D12_CPU_DESCRIPTOR_HANDLE FD3D12RenderWindow::GetRenderTargetView(uint32 index) const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        mDX12DescriptorHeapRenderTarget->GetCPUDescriptorHandleForHeapStart(),
        PosInHeap + index,
        mRTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE FD3D12RenderWindow::GetDepthStencilView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(mDX12DescriptorHeapDepthStencil->GetCPUDescriptorHandleForHeapStart(), PosInHeapDSV, mDSVDescriptorSize);
}

void FD3D12RenderWindow::Present()
{
    THROW_IF_FAILED(mDXGISwapChain->Present(0, 0));

    mRenderTargetIndex = (mRenderTargetIndex + 1) % NumTarget;
}
