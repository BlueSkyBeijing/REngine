#pragma once
#include "FRHIRenderTarget.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include "Utility.h"

#define SWAPCHAIN_BUFFER_COUNT 2

class FInputManager;

class FD3D12RenderTarget : FRHIRenderTarget
{
    friend class FD3D12RHI;

public:
	FD3D12RenderTarget(unsigned int width, unsigned int hight, DXGI_FORMAT format);
	~FD3D12RenderTarget();

    virtual void Init();
    virtual void UnInit();

public:
    uint32 Width;
    uint32 Height;
    DXGI_FORMAT Format;

private:
    virtual Microsoft::WRL::ComPtr <ID3D12Resource> GetRenderBuffer() const { return nullptr; };

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderBufferView() const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(); };
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(); };

protected:

};

class FRenderWindow : public FD3D12RenderTarget
{
    friend class FD3D12RHI;

public:
    FRenderWindow(unsigned int width, unsigned int hight, DXGI_FORMAT format);
    ~FRenderWindow();

    virtual void Init() override;
    virtual void UnInit() override;

    void Present();

private:
    virtual Microsoft::WRL::ComPtr <ID3D12Resource> GetRenderBuffer() const override;

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderBufferView() const override;
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const override;

private:
    Microsoft::WRL::ComPtr <IDXGISwapChain> mDXGISwapChain;
    Microsoft::WRL::ComPtr <ID3D12Resource> mRenderTargets[SWAPCHAIN_BUFFER_COUNT];
    Microsoft::WRL::ComPtr <ID3D12Resource> mDepthStencilBuffer;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapRenderTarget;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapDepthStencil;

    DXGI_FORMAT mBackBufferFormat;
    DXGI_FORMAT mDepthStencilFormat;

    HWND mWindowHandle;
    uint32 mSwapChainBufferCount;
    uint32 mChainBufferndex;
    uint32 mRTVDescriptorSize;
    uint32 mDSVDescriptorSize;
};
