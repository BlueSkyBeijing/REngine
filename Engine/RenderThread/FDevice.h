#pragma once
#include "Utility.h"
#include "FBuffer.h"

#include <wrl/client.h>
#include "d3d12.h"
#include <DirectXMath.h>
#include <string>
#include <dxgi1_6.h>

class FAdapter;
class FRenderProxy;
class FRenderTarget;
class FShader;
class FPipelineState;
class FVertexBuffer;
class FIndexBuffer;
class FRootSignature;
class FTexture2D;
class FRenderWindow;

class FDevice
{
    friend class FDeviceManager;

public:
	FDevice(Microsoft::WRL::ComPtr <ID3D12Device> inDXDevice);
	~FDevice();

    void Init();
    void UnInit();

    void BeginCommmandList();
    void EndCommmandList();

    void ExecuteCommandList();
    void FlushCommandQueue();

    void BeginDraw();
    void EndDraw();

    void Clear(const FVector4& color);

    void SetRenderTarget(FRenderTarget* renderTarget);
    void SetViewPort(FViewPort& viewPort);
    void SetSetScissor(FRect& scissorRect);

    void SetPipelineState(FPipelineState* pipelineState);

    void SetPrimitiveTopology(FPrimitiveTopology topology);
    void SetVertexBuffer(FVertexBuffer* buffer);
    void SetIndexBuffer(FIndexBuffer* buffer);
    void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation,int32 baseVertexLocation,uint32 startInstanceLocation);

    template <typename TBufferStruct>
    void CreateConstantBuffer(FConstantBuffer<TBufferStruct>* constantBuffer);
    void CreateVertexBuffer(FVertexBuffer* vertexBuffer);
    void CreateIndexBuffer(FIndexBuffer* indexBuffer);
    void CreateShader(FShader* shader);
    void CreateRootSignature(FRootSignature* rootSignature);
    void CreatePipelineState(FPipelineState* pipelineState);
    void CreateTexture(FTexture2D* texture2D);
    void CreateRenderTarget(FRenderWindow* renderTarget);

    void BeginEvent(std::string& eventName);
    void EndEvent();

    void Present();

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

    FAdapter* mAdapterParent;

    HANDLE mEventHandle;
    uint64 mFenceValue;
    D3D12_RECT mScissorRect;
    D3D12_VIEWPORT mViewPort;
};

#include "FDevice.inl"