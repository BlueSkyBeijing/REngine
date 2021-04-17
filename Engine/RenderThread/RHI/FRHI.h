#pragma once
#include "Prerequisite.h"

#include "FRHIBuffer.h"


enum EPrimitiveType
{
    PT_TriangleList,
    PT_TriangleStrip,
    PT_LineList,
    PT_PointList,
};

enum EPixelFormat
{
    PF_UNKNOWN = 0,
    PF_R32G32B32A32_TYPELESS = 1,
    PF_R32G32B32A32_FLOAT = 2,
    PF_R32G32B32A32_UINT = 3,
    PF_R32G32B32A32_SINT = 4,
    PF_R32G32B32_TYPELESS = 5,
    PF_R32G32B32_FLOAT = 6,
    PF_R32G32B32_UINT = 7,
    PF_R32G32B32_SINT = 8,
    PF_R16G16B16A16_TYPELESS = 9,
    PF_R16G16B16A16_FLOAT = 10,
    PF_R16G16B16A16_UNORM = 11,
    PF_R16G16B16A16_UINT = 12,
    PF_R16G16B16A16_SNORM = 13,
    PF_R16G16B16A16_SINT = 14,
    PF_R32G32_TYPELESS = 15,
    PF_R32G32_FLOAT = 16,
    PF_R32G32_UINT = 17,
    PF_R32G32_SINT = 18,
    PF_R32G8X24_TYPELESS = 19,
    PF_D32_FLOAT_S8X24_UINT = 20,
    PF_R32_FLOAT_X8X24_TYPELESS = 21,
    PF_X32_TYPELESS_G8X24_UINT = 22,
    PF_R10G10B10A2_TYPELESS = 23,
    PF_R10G10B10A2_UNORM = 24,
    PF_R10G10B10A2_UINT = 25,
    PF_R11G11B10_FLOAT = 26,
    PF_R8G8B8A8_TYPELESS = 27,
    PF_R8G8B8A8_UNORM = 28,
    PF_R8G8B8A8_UNORM_SRGB = 29,
    PF_R8G8B8A8_UINT = 30,
    PF_R8G8B8A8_SNORM = 31,
    PF_R8G8B8A8_SINT = 32,
    PF_R16G16_TYPELESS = 33,
    PF_R16G16_FLOAT = 34,
    PF_R16G16_UNORM = 35,
    PF_R16G16_UINT = 36,
    PF_R16G16_SNORM = 37,
    PF_R16G16_SINT = 38,
    PF_R32_TYPELESS = 39,
    PF_D32_FLOAT = 40,
    PF_R32_FLOAT = 41,
    PF_R32_UINT = 42,
    PF_R32_SINT = 43,
    PF_R24G8_TYPELESS = 44,
    PF_D24_UNORM_S8_UINT = 45,
    PF_R24_UNORM_X8_TYPELESS = 46,
    PF_X24_TYPELESS_G8_UINT = 47,
    PF_R8G8_TYPELESS = 48,
    PF_R8G8_UNORM = 49,
    PF_R8G8_UINT = 50,
    PF_R8G8_SNORM = 51,
    PF_R8G8_SINT = 52,
    PF_R16_TYPELESS = 53,
    PF_R16_FLOAT = 54,
    PF_D16_UNORM = 55,
    PF_R16_UNORM = 56,
    PF_R16_UINT = 57,
    PF_R16_SNORM = 58,
    PF_R16_SINT = 59,
    PF_R8_TYPELESS = 60,
    PF_R8_UNORM = 61,
    PF_R8_UINT = 62,
    PF_R8_SNORM = 63,
    PF_R8_SINT = 64,
    PF_A8_UNORM = 65,
    PF_R1_UNORM = 66,
    PF_R9G9B9E5_SHAREDEXP = 67,
    PF_R8G8_B8G8_UNORM = 68,
    PF_G8R8_G8B8_UNORM = 69,
    PF_BC1_TYPELESS = 70,
    PF_BC1_UNORM = 71,
    PF_BC1_UNORM_SRGB = 72,
    PF_BC2_TYPELESS = 73,
    PF_BC2_UNORM = 74,
    PF_BC2_UNORM_SRGB = 75,
    PF_BC3_TYPELESS = 76,
    PF_BC3_UNORM = 77,
    PF_BC3_UNORM_SRGB = 78,
    PF_BC4_TYPELESS = 79,
    PF_BC4_UNORM = 80,
    PF_BC4_SNORM = 81,
    PF_BC5_TYPELESS = 82,
    PF_BC5_UNORM = 83,
    PF_BC5_SNORM = 84,
    PF_B5G6R5_UNORM = 85,
    PF_B5G5R5A1_UNORM = 86,
    PF_B8G8R8A8_UNORM = 87,
    PF_B8G8R8X8_UNORM = 88,
    PF_R10G10B10_XR_BIAS_A2_UNORM = 89,
    PF_B8G8R8A8_TYPELESS = 90,
    PF_B8G8R8A8_UNORM_SRGB = 91,
    PF_B8G8R8X8_TYPELESS = 92,
    PF_B8G8R8X8_UNORM_SRGB = 93,
    PF_BC6H_TYPELESS = 94,
    PF_BC6H_UF16 = 95,
    PF_BC6H_SF16 = 96,
    PF_BC7_TYPELESS = 97,
    PF_BC7_UNORM = 98,
    PF_BC7_UNORM_SRGB = 99,
    PF_AYUV = 100,
    PF_Y410 = 101,
    PF_Y416 = 102,
    PF_NV12 = 103,
    PF_P010 = 104,
    PF_P016 = 105,
    PF_420_OPAQUE = 106,
    PF_YUY2 = 107,
    PF_Y210 = 108,
    PF_Y216 = 109,
    PF_NV11 = 110,
    PF_AI44 = 111,
    PF_IA44 = 112,
    PF_P8 = 113,
    PF_A8P8 = 114,
    PF_B4G4R4A4_UNORM = 115,

    PF_P208 = 130,
    PF_V208 = 131,
    PF_V408 = 132,

    PF_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
    PF_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,

    PF_FORCE_UINT = 0xffffffff
};

enum ERHIAccess
{
    ACCESS_COMMON = 0,
    ACCESS_VERTEX_AND_CONSTANT_BUFFER = 0x1,
    ACCESS_INDEX_BUFFER = 0x2,
    ACCESS_RENDER_TARGET = 0x4,
    ACCESS_UNORDERED_ACCESS = 0x8,
    ACCESS_DEPTH_WRITE = 0x10,
    ACCESS_DEPTH_READ = 0x20,
    ACCESS_NON_PIXEL_SHADER_RESOURCE = 0x40,
    ACCESS_PIXEL_SHADER_RESOURCE = 0x80,
    ACCESS_STREAM_OUT = 0x100,
    ACCESS_INDIRECT_ARGUMENT = 0x200,
    ACCESS_COPY_DEST = 0x400,
    ACCESS_COPY_SOURCE = 0x800,
    ACCESS_RESOLVE_DEST = 0x1000,
    ACCESS_RESOLVE_SOURCE = 0x2000,
    ACCESS_RAYTRACING_ACCELERATION_STRUCTURE = 0x400000,
    ACCESS_SHADING_RATE_SOURCE = 0x1000000,
    ACCESS_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
    ACCESS_PRESENT = 0,
    ACCESS_PREDICATION = 0x200,
};

struct FRHITransitionInfo
{
    FRHIResource* Resource;
    ERHIAccess AccessBefore;
    ERHIAccess AccessAfter;

    FRHITransitionInfo(
        FRHIResource* InResource,
        ERHIAccess InPreviousState,
        ERHIAccess InNewState)
        : Resource(InResource)
        , AccessBefore(InPreviousState)
        , AccessAfter(InNewState)
    {}
};

class FRHI
{
public:
    FRHI() {}
    virtual ~FRHI() {}

    virtual void Init() = 0;
    virtual void UnInit() = 0;

    virtual void BeginCommmandList() = 0;
    virtual void EndCommmandList() = 0;
    virtual void ExecuteCommandList() = 0;
    virtual void FlushCommandQueue() = 0;

    virtual void Clear(const FVector4& color) = 0;

    virtual void SetRenderTarget(FRHIRenderTarget* renderTarget) = 0;
    virtual void SetViewPort(FViewPort& viewPort) = 0;
    virtual void SetSetScissor(FRect& scissorRect) = 0;

    virtual void SetPipelineState(FRHIPipelineState* pipelineState) = 0;

    virtual void SetPrimitiveType(EPrimitiveType primitiveType) = 0;
    virtual void SetVertexBuffer(FRHIVertexBuffer* buffer) = 0;
    virtual void SetIndexBuffer(FRHIIndexBuffer* buffer) = 0;
    virtual void SetConstantBuffer(FRHIConstantBuffer* buffer) = 0;
    virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, int32 baseVertexLocation, uint32 startInstanceLocation) = 0;

    virtual FRHIConstantBuffer* CreateConstantBuffer(uint32 structureSize, uint8* bufferData, int32 slot) = 0;
    virtual FRHIVertexBuffer* CreateVertexBuffer(uint32 structureSize, uint32 vertexCount, uint8* bufferData) = 0;
    virtual FRHIIndexBuffer* CreateIndexBuffer(uint32 structureSize, uint32 indexCount, uint8* bufferData) = 0;
    virtual FRHIShader* CreateShader(const std::wstring& filePathName, const std::string& EnterPoint, const std::string& target) = 0;
    virtual FRHIShaderBindings* CreateShaderBindings() = 0;
    virtual FRHIPipelineState* CreatePipelineState(FRHIShaderBindings* shaderBindings, FRHIShader* vertexShader, FRHIShader* pixelShader, FRHIVertexLayout* vertexLayout) = 0;
    virtual FRHITexture2D* CreateTexture2D(const std::wstring& filePathName, int32 slot) = 0;
    virtual FRHIRenderWindow* CreateRenderWindow(uint32 width, uint32 hight) = 0;

    virtual void UpdateConstantBuffer(FRHIConstantBuffer* constantBuffer, uint32 structureSize, uint8* bufferData) = 0;

    virtual void Transition(const FRHITransitionInfo& info) = 0;

    virtual void BeginEvent(std::string& eventName) = 0;
    virtual void EndEvent() = 0;

    virtual void Present() = 0;

protected:

private:
};
