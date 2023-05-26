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

typedef std::string FString;

template<class T>
using TArray = std::vector<T>;

template<class T1, class T2>
using TMap = std::map<T1, T2>;

template<class T>
using TSet = std::set<T>;

template<class T>
using TSharedPtr = std::shared_ptr<T>;

template<class T>
using TWeakPtr = std::weak_ptr<T>;

template<class T>
using TUniquePtr = std::unique_ptr<T>;

template<class T, class... Args>
std::shared_ptr<T> TMakeShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<class T, class... Args>
std::unique_ptr<T> TMakeUnique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class FThread;

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
class IModuleInterface;

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
class FD3D12Texture;
class FD3D12Texture;

#define FRAME_BUFFER_NUM 2

#ifdef DLL_EXPORTS
#define DLL_API  _declspec(dllexport)
#else
#define DLL_API  _declspec(dllimport)
#endif


#define DECLARE_SHAREDPTR(ClassName) \
	typedef TSharedPtr<ClassName> ClassName##Ptr;


DECLARE_SHAREDPTR(UObject)
DECLARE_SHAREDPTR(UWorld)
DECLARE_SHAREDPTR(UTexture2D)
DECLARE_SHAREDPTR(UTextureCube)
DECLARE_SHAREDPTR(UStaticMeshObject)
DECLARE_SHAREDPTR(USkeletalMeshObject)
DECLARE_SHAREDPTR(UAnimSequence)
DECLARE_SHAREDPTR(USkeleton)
DECLARE_SHAREDPTR(UPlayer)
DECLARE_SHAREDPTR(UCamera)
DECLARE_SHAREDPTR(USkeletalMeshObject)
DECLARE_SHAREDPTR(USkeletalMesh)
DECLARE_SHAREDPTR(UMaterial)
DECLARE_SHAREDPTR(UResource)
DECLARE_SHAREDPTR(UPointLight)
DECLARE_SHAREDPTR(UDirectionalLight)

DECLARE_SHAREDPTR(FEngine)
DECLARE_SHAREDPTR(FRenderThread)
DECLARE_SHAREDPTR(FRenderer)
DECLARE_SHAREDPTR(FScene)
DECLARE_SHAREDPTR(FView)
DECLARE_SHAREDPTR(FViewPort)
DECLARE_SHAREDPTR(FRenderProxy)
DECLARE_SHAREDPTR(FMeshBatch)
DECLARE_SHAREDPTR(FMaterial)
DECLARE_SHAREDPTR(FDirectionalLight)
DECLARE_SHAREDPTR(FShaderInfo)
DECLARE_SHAREDPTR(FPipelineStateInfo)
DECLARE_SHAREDPTR(FStaticMeshRenderProxy)
DECLARE_SHAREDPTR(FSkeletalMeshRenderProxy)
DECLARE_SHAREDPTR(FAnimSequenceBlender)
DECLARE_SHAREDPTR(FResourceManager)
DECLARE_SHAREDPTR(FAnimSequenceInstance)
DECLARE_SHAREDPTR(FLight)
DECLARE_SHAREDPTR(FPointLight)
DECLARE_SHAREDPTR(IModuleInterface)

DECLARE_SHAREDPTR(FSceneManager)
DECLARE_SHAREDPTR(FShaderManager)
DECLARE_SHAREDPTR(FInputManager)
DECLARE_SHAREDPTR(FShaderBindingsManager)
DECLARE_SHAREDPTR(FPipelineStateManager)
DECLARE_SHAREDPTR(FShaderManager)
DECLARE_SHAREDPTR(FShaderBindingsManager)
DECLARE_SHAREDPTR(FPipelineStateManager)
DECLARE_SHAREDPTR(FRenderCommand)
DECLARE_SHAREDPTR(FPostProcessing)
DECLARE_SHAREDPTR(FScreenVertex)
DECLARE_SHAREDPTR(FPlayerController)
DECLARE_SHAREDPTR(FConsoleVariableManager)

DECLARE_SHAREDPTR(FRHI)
DECLARE_SHAREDPTR(FRHIResource)
DECLARE_SHAREDPTR(FRHIShader)
DECLARE_SHAREDPTR(FRHITexture)
DECLARE_SHAREDPTR(FRHIBuffer)
DECLARE_SHAREDPTR(FRHIIndexBuffer)
DECLARE_SHAREDPTR(FRHIVertexBuffer)
DECLARE_SHAREDPTR(FRHIIndexBuffer)
DECLARE_SHAREDPTR(FRHIRenderTarget)
DECLARE_SHAREDPTR(FRHIShader)
DECLARE_SHAREDPTR(FRHIPipelineState)
DECLARE_SHAREDPTR(FRHIVertexBuffer)
DECLARE_SHAREDPTR(FRHIIndexBuffer)
DECLARE_SHAREDPTR(FRHIShaderBindings)
DECLARE_SHAREDPTR(FRHIRenderWindow)
DECLARE_SHAREDPTR(FRHIRenderTarget)
DECLARE_SHAREDPTR(FRHIVertexLayout)
DECLARE_SHAREDPTR(FRHIConstantBuffer)

DECLARE_SHAREDPTR(FD3D12ShaderBindings)
DECLARE_SHAREDPTR(FD3D12Shader)
DECLARE_SHAREDPTR(FD3D12VertexLayout)
DECLARE_SHAREDPTR(FD3D12RenderWindow)
DECLARE_SHAREDPTR(FD3D12RenderTarget)
DECLARE_SHAREDPTR(FD3D12PipelineState)
DECLARE_SHAREDPTR(FD3D12VertexBuffer)
DECLARE_SHAREDPTR(FD3D12ShaderBindings)
DECLARE_SHAREDPTR(FD3D12Texture)
DECLARE_SHAREDPTR(FD3D12Texture)
