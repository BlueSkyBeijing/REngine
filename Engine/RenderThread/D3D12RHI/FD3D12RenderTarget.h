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

protected:
};

class FD3D12RenderWindow : public FRHIRenderWindow
{
    friend class FD3D12RHI;

public:
    FD3D12RenderWindow(uint32 width, uint32 hight, DXGI_FORMAT format, uint32 numTarget);
    ~FD3D12RenderWindow();

public:
    virtual void Init() override;
    virtual void UnInit() override;

    virtual void Present() override;

private:
    Microsoft::WRL::ComPtr <IDXGISwapChain> mDXGISwapChain;
};
