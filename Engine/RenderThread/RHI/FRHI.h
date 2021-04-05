#pragma once
#include "Utility.h"
#include "FRHIBuffer.h"

class FRHIRenderTarget;
class FRHIShader;
class FRHIPipelineState;
class FRHIVertexBuffer;
class FRHIIndexBuffer;
class FRHIShaderBindings;
class FRHITexture2D;
class FRHIRenderWindow;


enum EPrimitiveTopology
{
    PRIMITIVE_TOPOLOGY_UNDEFINED = 0,
    PRIMITIVE_TOPOLOGY_POINTLIST = 1,
    PRIMITIVE_TOPOLOGY_LINELIST = 2,
    PRIMITIVE_TOPOLOGY_LINESTRIP = 3,
    PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
    PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 5,
};

class FRHI
{
public:
	FRHI(){}
    virtual ~FRHI(){}

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

    virtual void SetPrimitiveTopology(EPrimitiveTopology topology) = 0;
    virtual void SetVertexBuffer(FRHIVertexBuffer* buffer) = 0;
    virtual void SetIndexBuffer(FRHIIndexBuffer* buffer) = 0;
    virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation,int32 baseVertexLocation,uint32 startInstanceLocation) = 0;

    virtual FRHIConstantBuffer* CreateConstantBuffer(uint32 structureSize, uint8* bufferData, int32 slot) = 0;
    virtual FRHIVertexBuffer* CreateVertexBuffer(uint32 structureSize, uint32 vertexCount, uint8* bufferData) = 0;
    virtual FRHIIndexBuffer* CreateIndexBuffer(uint32 structureSize, uint32 indexCount, uint8* bufferData) = 0;
    virtual FRHIShader* CreateShader(const std::wstring& filePathName, const std::string& EnterPoint, const std::string& target) = 0;
    virtual FRHIShaderBindings* CreateShaderBindings() = 0;
    virtual FRHIPipelineState* CreatePipelineState(FRHIShaderBindings* shaderBindings, FRHIShader* vertexShader, FRHIShader* pixelShader, FRHIVertexLayout* vertexLayout) = 0;
    virtual FRHITexture2D* CreateTexture2D(const std::wstring& filePathName, int32 slot) = 0;
    virtual FRHIRenderWindow* CreateRenderWindow(uint32 width, uint32 hight) = 0;

    virtual void BeginEvent(std::string& eventName) = 0;
    virtual void EndEvent() = 0;

    virtual void Present() = 0;

protected:

private:
};
