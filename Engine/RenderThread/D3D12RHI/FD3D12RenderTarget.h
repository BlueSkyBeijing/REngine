#pragma once
#include "Prerequisite.h"

#include "FRHIRenderTarget.h"

#define SWAPCHAIN_BUFFER_COUNT 2


class FD3D12RenderTarget : FRHIRenderTarget
{
    friend class FD3D12RHI;

public:
	FD3D12RenderTarget(unsigned int width, unsigned int hight, DXGI_FORMAT format);
	~FD3D12RenderTarget();

    virtual void Init();
    virtual void UnInit();

public:
    DXGI_FORMAT Format;

private:
    virtual Microsoft::WRL::ComPtr <ID3D12Resource> GetRenderBuffer() const { return nullptr; };

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderBufferView() const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(); };
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(); };

protected:

};

class FD3D12RenderWindow : public FRHIRenderWindow
{
    friend class FD3D12RHI;

public:
    FD3D12RenderWindow(unsigned int width, unsigned int hight, DXGI_FORMAT format);
    ~FD3D12RenderWindow();

    virtual void Init() override;
    virtual void UnInit() override;

    virtual void Present() override;

private:
    virtual Microsoft::WRL::ComPtr <ID3D12Resource> GetRenderBuffer() const ;

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderBufferView() const ;
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const ;

private:
    Microsoft::WRL::ComPtr <IDXGISwapChain> mDXGISwapChain;
    Microsoft::WRL::ComPtr <ID3D12Resource> mRenderTargets[SWAPCHAIN_BUFFER_COUNT];
    Microsoft::WRL::ComPtr <ID3D12Resource> mDepthStencilBuffer;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapRenderTarget;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapDepthStencil;

    DXGI_FORMAT mBackBufferFormat;
    DXGI_FORMAT mDepthStencilFormat;

    uint32 mSwapChainBufferCount;
    uint32 mChainBufferndex;
    uint32 mRTVDescriptorSize;
    uint32 mDSVDescriptorSize;
};
