#pragma once
#include "Utility.h"
#include "FD3D12Buffer.h"
#include "FRHI.h"

#include <wrl/client.h>
#include "d3d12.h"
#include <DirectXMath.h>
#include <string>
#include <dxgi1_6.h>

class FRenderProxy;
class FRenderWindow;
class FD3D12RenderTarget;
class FD3D12Shader;
class FD3D12PipelineState;
class FD3D12VertexBuffer;
class FRHIIndexBuffer;
class FD3D12ShaderBindings;
class FD3D12Texture2D;

class FD3D12RHI : public FRHI
{
public:
	FD3D12RHI();
    virtual ~FD3D12RHI() override;

    virtual void Init() override;
    virtual void UnInit() override;

    virtual void BeginCommmandList() override;
    virtual void EndCommmandList() override;

    virtual void ExecuteCommandList() override;
    virtual void FlushCommandQueue() override;

    virtual void BeginDraw() override;
    virtual void EndDraw() override;

    virtual void Clear(const FVector4& color) override;

    virtual void SetRenderTarget(FRHIRenderTarget* renderTarget) override;
    virtual void SetViewPort(FViewPort& viewPort) override;
    virtual void SetSetScissor(FRect& scissorRect) override;

    virtual void SetPipelineState(FRHIPipelineState* pipelineState) override;

    virtual void SetPrimitiveTopology(FPrimitiveTopology topology) override;
    virtual void SetVertexBuffer(FRHIVertexBuffer* buffer) override;
    virtual void SetIndexBuffer(FRHIIndexBuffer* buffer) override;
    virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation,int32 baseVertexLocation,uint32 startInstanceLocation) override;

    template <typename TBufferStruct>
    virtual void CreateConstantBuffer(FRHIConstantBuffer<TBufferStruct>* constantBuffer) override;
    virtual void CreateVertexBuffer(FRHIVertexBuffer* vertexBuffer) override;
    virtual void CreateIndexBuffer(FRHIIndexBuffer* indexBuffer) override;
    virtual void CreateShader(FRHIShader* shader) override;
    virtual void CreateRootSignature(FRHIShaderBindings* rootSignature) override;
    virtual void CreatePipelineState(FRHIPipelineState* pipelineState) override;
    virtual void CreateTexture(FRHITexture2D* texture2D) override;
    virtual void CreateRenderTarget(FRenderWindow* renderTarget) override;

    virtual void BeginEvent(std::string& eventName) override;
    virtual void EndEvent() override;

    virtual void Present() override;

protected:

private:
    FRenderWindow* mRenderTarget;

    Microsoft::WRL::ComPtr <IDXGIFactory2> mDXGIFactory;
    Microsoft::WRL::ComPtr <IDXGISwapChain> mDXGISwapChain;
    Microsoft::WRL::ComPtr <ID3D12Device> mDX12Device;
    Microsoft::WRL::ComPtr <ID3D12CommandQueue> mDX12CommandQueue;
    Microsoft::WRL::ComPtr <ID3D12CommandAllocator> mDX12CommandAllocator;
    Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> mDX12CommandList;
    Microsoft::WRL::ComPtr <ID3D12Fence> mDX12Fence;

    Microsoft::WRL::ComPtr <ID3D12RootSignature> mDX12RootSignature;
    Microsoft::WRL::ComPtr <ID3D12PipelineState> mDX12PipleLineState;

    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mCBVSRVUAVHeap;

	Microsoft::WRL::ComPtr <ID3D12Resource> mCurTexture;
	Microsoft::WRL::ComPtr <ID3D12Resource> mCurTextureUploadHeap;

    DXGI_FORMAT mBackBufferFormat;
    DXGI_FORMAT mDepthStencilFormat;

	uint32 mRTVDescriptorSize;
    uint32 mDSVDescriptorSize;
    uint32 mCBVSRVVUAVDescriptorSize;

    HANDLE mEventHandle;
    uint64 mFenceValue;
    D3D12_RECT mScissorRect;
    D3D12_VIEWPORT mViewPort;
};

#include "FD3D12RHI.inl"