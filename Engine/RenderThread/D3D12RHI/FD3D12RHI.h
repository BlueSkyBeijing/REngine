#pragma once
#include "Prerequisite.h"

#include "FD3D12Buffer.h"
#include "FRHI.h"


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

    virtual void Clear(bool clearColor, const FVector4& color, bool clearDepth, float depth, bool clearStencil, uint32 stencil) override;

    virtual void SetRenderTarget(FRHIRenderTarget* renderTarget) override;
    virtual void SetViewPort(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) override;
    virtual void SetSetScissor(bool enable, float minX, float minY, float maxX, float maxY) override;

    virtual void SetPipelineState(FRHIPipelineState* pipelineState) override;

    virtual void SetPrimitiveType(EPrimitiveType primitiveType) override;
    virtual void SetVertexBuffer(FRHIVertexBuffer* buffer) override;
    virtual void SetIndexBuffer(FRHIIndexBuffer* buffer) override;
    virtual void SetConstantBuffer(FRHIConstantBuffer* buffer, int32 shaderPos) override;
    virtual void SetTexture2D(FRHITexture2D* texture, int32 shaderPos) override;

    virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, int32 baseVertexLocation, uint32 startInstanceLocation) override;

    virtual FRHIConstantBuffer* CreateConstantBuffer(uint32 structureSize, uint8* bufferData) override;
    virtual FRHIVertexBuffer* CreateVertexBuffer(uint32 structureSize, uint32 vertexCount, uint8* bufferData) override;
    virtual FRHIIndexBuffer* CreateIndexBuffer(uint32 structureSize, uint32 indexCount, uint8* bufferData) override;
    virtual FRHIShader* CreateShader(const std::wstring& filePathName, const std::string& enterPoint, const std::string& target) override;
    virtual FRHIShaderBindings* CreateShaderBindings() override;
    virtual FRHIPipelineState* CreatePipelineState(const FPipelineStateInfo& info) override;
    virtual FRHITexture2D* CreateTexture2D(const std::wstring& filePathName) override;
    virtual FRHIRenderTarget* CreateRenderTarget(uint32 width, uint32 hight, uint32 numTarget, EPixelFormat formatTarget, EPixelFormat formatDepthStencil) override;
    virtual FRHIRenderWindow* CreateRenderWindow(uint32 width, uint32 hight) override;

    virtual void UpdateConstantBuffer(FRHIConstantBuffer* constantBuffer, uint32 structureSize, uint8* bufferData) override;

    virtual void TransitionResource(const FRHITransitionInfo& info) override;

    virtual void BeginEvent(const char* eventName) override;
    virtual void EndEvent() override;

    virtual void BeginRenderPass(const FRHIRenderPassInfo& info, const char* name) override;
    virtual void EndRenderPass() override;

    virtual void Present(FRHIRenderWindow* window) override;

protected:
    void createHeaps();

    D3D_PRIMITIVE_TOPOLOGY translatePrimitiveType(EPrimitiveType primitiveType);
    D3D12_RESOURCE_STATES translateResourceTransitionAccess(EResourceTransitionAccess access);
    DXGI_FORMAT translatePixelFormat(EPixelFormat format);

private:
    FRHIRenderTarget* mRenderTargetCurrent;

    Microsoft::WRL::ComPtr <IDXGIFactory2> mDXGIFactory;
    Microsoft::WRL::ComPtr <IDXGISwapChain> mDXGISwapChain;
    Microsoft::WRL::ComPtr <ID3D12Device> mDX12Device;
    Microsoft::WRL::ComPtr <ID3D12CommandQueue> mDX12CommandQueue;
    Microsoft::WRL::ComPtr <ID3D12CommandAllocator> mDX12CommandAllocator[FRAME_BUFFER_NUM];
    Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> mDX12CommandList;
    Microsoft::WRL::ComPtr <ID3D12Fence> mDX12Fence;

    Microsoft::WRL::ComPtr <ID3D12RootSignature> mDX12RootSignature;
    Microsoft::WRL::ComPtr <ID3D12PipelineState> mDX12PipleLineState;

    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapRenderTarget;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mDX12DescriptorHeapDepthStencil;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> mCBVSRVUAVHeap;

    Microsoft::WRL::ComPtr <ID3D12Resource> mCurTexture;
    Microsoft::WRL::ComPtr <ID3D12Resource> mCurTextureUploadHeap;

    DXGI_FORMAT mBackBufferFormat;
    DXGI_FORMAT mDepthStencilFormat;

    uint32 mRTVDescriptorSize;
    uint32 mDSVDescriptorSize;
    uint32 mCBVSRVVUAVDescriptorSize;

    HANDLE mEventHandle;
    uint64 mFenceValue[FRAME_BUFFER_NUM];
    D3D12_RECT mScissorRect;
    D3D12_VIEWPORT mViewPort;
    uint32 mFrameIndex;

    static int32 msPassSRVTableIndex;
    static int32 msPassCBVTableIndex;
    static int32 msObjectSRVTableIndex;
    static int32 msObjectCBVTableIndex;

    static int32 msRTVCount;
    static int32 msDSVCount;
    static int32 msCBVSRVUAVCount;

};
