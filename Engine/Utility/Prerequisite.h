#pragma once

typedef uint8_t     uint8;
typedef uint16_t    uint16;
typedef uint32_t    uint32;
typedef uint64_t    uint64;
typedef int8_t      int8;
typedef int16_t     int16;
typedef int32_t     int32;
typedef int64_t     int64;

typedef Eigen::Vector4f FVector4;
typedef Eigen::Vector3f FVector3;
typedef Eigen::Vector2f FVector2;
typedef Eigen::Vector2i FVector2i;

typedef Eigen::Matrix4f FMatrix4x4;
typedef Eigen::Matrix3f FMatrix3x3;

typedef Eigen::Quaternionf FQuat;

typedef Eigen::Vector4f FLinearColor;

enum EPixelFormat;
enum EInputClassification;
enum ELogLevel;
enum EResourceType;
enum EBlendMode;
enum EMaterialShadingModel;

struct FRect;
struct FInputElementDesc;
struct FTransform;

class UObject;
class UWorld;
class UTexture2D;
class UTextureCube;
class UStaticMeshObject;
class USkeletalMeshObject;
class UAnimSequence;
class USkeleton;
class UPlayer;
class UCamera;
class USkeletalMeshObject;
class USkeletalMesh;
class UMaterial;
class UResource;
class UPointLight;
class UDirectionalLight;

class FEngine;
class FRenderThread;
class FRenderer;
class FScene;
class FView;
class FViewPort;
class FRenderProxy;
class FMeshBatch;
class FMaterial;
class FDirectionalLight;
class FShaderInfo;
class FPipelineStateInfo;
class FStaticMeshRenderProxy;
class FSkeletalMeshRenderProxy;
class FAnimSequenceBlender;
class FResourceManager;
class FAnimSequenceInstance;
class FLight;
class FPointLight;

class FSceneManager;
class FShaderManager;
class FInputManager;
class FShaderBindingsManager;
class FPipelineStateManager;
class FShaderManager;
class FShaderBindingsManager;
class FPipelineStateManager;
class FRenderCommand;
class FPostProcessing;
class FScreenVertex;
class FPlayerController;
class FConsoleVariableManager;

class FRHI;
class FRHIResource;
class FRHIShader;
class FRHITexture;
class FRHITexture2D;
class FRHITextureCube;
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
class FRHIVertexLayout;
class FRHIConstantBuffer;

class FD3D12ShaderBindings;
class FD3D12Shader;
class FD3D12VertexLayout;
class FD3D12RenderWindow;
class FD3D12RenderTarget;
class FD3D12PipelineState;
class FD3D12VertexBuffer;
class FD3D12ShaderBindings;
class FD3D12Texture2D;
class FD3D12TextureCube;

#define FRAME_BUFFER_NUM 2
