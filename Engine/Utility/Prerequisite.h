#pragma once

typedef uint8_t	    uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;
typedef	int8_t		int8;
typedef int16_t	    int16;
typedef int32_t   	int32;
typedef int64_t	    int64;

typedef Eigen::Vector4f FVector4;
typedef Eigen::Vector3f FVector3;
typedef Eigen::Vector2f FVector2;

typedef Eigen::Matrix4f FMatrix4x4;

enum EPixelFormat;
enum EInputClassification;

struct FViewPort;
struct FRect;
struct FInputElementDesc;

class UObject;
class UWorld;
class UTexture2D;

class FEngine;
class FRenderThread;
class FRenderer;
class FScene;
class FView;
class FRenderProxy;
class FMaterial;

class FSceneManager;
class FShaderManager;
class FInputManager;
class FShaderBindingsManager;
class FPipelineStateManager;
class FShaderManager;
class FShaderBindingsManager;
class FPipelineStateManager;

class FRHIShader;
class FRHITexture2D;
class FRHIBuffer;
class FRHIIndexBuffer;
class FRHIVertexBuffer;
class FRHIIndexBuffer;
class FRHIRenderTarget;
class FRHIShader;
class FRHIPipelineState;
class FRHIVertexBuffer;
class FRHIIndexBuffer;
class FRHIShaderBindings;
class FRHITexture2D;
class FRHIRenderWindow;
class FRHIRenderTarget;

class FD3D12ShaderBindings;
class FD3D12Shader;
class FD3D12VertexLayout;
class FD3D12RenderWindow;
class FD3D12RenderTarget;
class FD3D12PipelineState;
class FD3D12VertexBuffer;
class FD3D12ShaderBindings;
class FD3D12Texture2D;
