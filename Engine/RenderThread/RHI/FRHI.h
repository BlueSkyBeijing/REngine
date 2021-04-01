#pragma once
#include "Utility.h"
#include "FRHIBuffer.h"

#include <string>

class FRHIRenderTarget;
class FRHIShader;
class FRHIPipelineState;
class FRHIVertexBuffer;
class FRHIIndexBuffer;
class FRHIShaderBindings;
class FRHITexture2D;

class FRHI
{
public:
	FRHI() = 0;
    virtual ~FRHI() = 0;

    virtual void Init() = 0;
    virtual void UnInit() = 0;

    virtual void BeginCommmandList() = 0;
    virtual void EndCommmandList() = 0;

    virtual void ExecuteCommandList() = 0;
    virtual void FlushCommandQueue() = 0;

    virtual void BeginDraw() = 0;
    virtual void EndDraw() = 0;

    virtual void Clear(const FVector4& color) = 0;

    virtual void SetRenderTarget(FRHIRenderTarget* renderTarget) = 0;
    virtual void SetViewPort(FViewPort& viewPort) = 0;
    virtual void SetSetScissor(FRect& scissorRect) = 0;

    virtual void SetPipelineState(FRHIPipelineState* pipelineState) = 0;

    virtual void SetPrimitiveTopology(FPrimitiveTopology topology) = 0;
    virtual void SetVertexBuffer(FRHIVertexBuffer* buffer) = 0;
    virtual void SetIndexBuffer(FRHIIndexBuffer* buffer) = 0;
    virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation,int32 baseVertexLocation,uint32 startInstanceLocation) = 0;

    template <typename TBufferStruct>
    virtual void CreateConstantBuffer(FRHIConstantBuffer<TBufferStruct>* constantBuffer) = 0;
    virtual void CreateVertexBuffer(FRHIVertexBuffer* vertexBuffer) = 0;
    virtual void CreateIndexBuffer(FRHIIndexBuffer* indexBuffer) = 0;
    virtual void CreateShader(FRHIShader* shader) = 0;
    virtual void CreateRootSignature(FRHIShaderBindings* rootSignature) = 0;
    virtual void CreatePipelineState(FRHIPipelineState* pipelineState) = 0;
    virtual void CreateTexture(FRHITexture2D* texture2D) = 0;
    virtual void CreateRenderTarget(FRenderWindow* renderTarget) = 0;

    virtual void BeginEvent(std::string& eventName) = 0;
    virtual void EndEvent() = 0;

    virtual void Present() = 0;

protected:

private:
};
