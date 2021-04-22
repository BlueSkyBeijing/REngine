#pragma once
#include "Prerequisite.h"

#include "FRHIRenderTarget.h"


class FD3D12RenderTarget : public FRHIRenderTarget
{
    friend class FD3D12RHI;

public:
    FD3D12RenderTarget(uint32 width, uint32 hight, uint32 numTarget);
    ~FD3D12RenderTarget();

    virtual void Init();
    virtual void UnInit();

    virtual Microsoft::WRL::ComPtr <ID3D12Resource> GetRenderTargetBuffer(uint32 index) const;
    virtual Microsoft::WRL::ComPtr <ID3D12Resource> GetDepthStencilBuffer() const;

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView(uint32 index) const;
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

    uint32 GetRenderTargetIndex() const
    {
        return mRenderTargetIndex;
    }

protected:
    std::vector<Microsoft::WRL::ComPtr <ID3D12Resource>> mRenderTargets;
    Microsoft::WRL::ComPtr <ID3D12Resource> mDepthStencilBuffer;
    DXGI_FORMAT mRenderTargetFormat;
    DXGI_FORMAT mDepthStencilFormat;
    uint32 mRenderTargetIndex;

    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapRenderTarget;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapDepthStencil;
    uint32 mRTVDescriptorSize;
    uint32 mDSVDescriptorSize;
};

class FD3D12RenderWindow : public FRHIRenderWindow
{
    friend class FD3D12RHI;

public:
    FD3D12RenderWindow(uint32 width, uint32 hight, DXGI_FORMAT format, uint32 numTarget);
    ~FD3D12RenderWindow();

    virtual void Init() override;
    virtual void UnInit() override;

    virtual void Present() override;

    virtual Microsoft::WRL::ComPtr <ID3D12Resource> GetRenderTargetBuffer(uint32 index) const;
    virtual Microsoft::WRL::ComPtr <ID3D12Resource> GetDepthStencilBuffer() const;

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView(uint32 index) const;
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

    uint32 GetRenderTargetIndex() const
    {
        return mRenderTargetIndex;
    }

private:
    std::vector<Microsoft::WRL::ComPtr <ID3D12Resource>> mRenderTargets;
    Microsoft::WRL::ComPtr <ID3D12Resource> mDepthStencilBuffer;
    DXGI_FORMAT mRenderTargetFormat;
    DXGI_FORMAT mDepthStencilFormat;
    uint32 mRenderTargetIndex;

    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapRenderTarget;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapDepthStencil;
    uint32 mRTVDescriptorSize;
    uint32 mDSVDescriptorSize;

    Microsoft::WRL::ComPtr <IDXGISwapChain> mDXGISwapChain;
};
