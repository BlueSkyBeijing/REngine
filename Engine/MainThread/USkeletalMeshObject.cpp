#include "PrecompiledHeader.h"

#include "USkeletalMeshObject.h"
#include "USkeletalMesh.h"
#include "UMaterial.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "FRenderProxy.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FResourceManager.h"
#include "FRenderCommand.h"

USkeletalMeshObject::USkeletalMeshObject() :
    mMaterial(nullptr),
    mSkeletalMesh(nullptr)
{
}

USkeletalMeshObject::~USkeletalMeshObject()
{
}

void USkeletalMeshObject::Load()
{
    mSkeletalMesh = dynamic_cast<USkeletalMesh*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_SkeletalMesh, FullResourcePath));

    mMaterial = dynamic_cast<UMaterial*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Material, FullMaterialPath));

    mAnimSequence = dynamic_cast<UAnimSequence*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Animation, FullAnimSequencePath));
    mAnimSequence->SeSkeleton(mSkeletalMesh->GetSkeleton());
    mAnimSequence->Load();

    //create render proxy
    createRenderProxy();
}

void USkeletalMeshObject::Unload()
{
    mSkeletalMesh = nullptr;

    mMaterial = nullptr;

    mAnimSequence = nullptr;
}

void USkeletalMeshObject::Update(float deltaSeconds)
{
    mAnimSequence->Update(deltaSeconds);

    dynamic_cast<FSkeletalMeshRenderProxy*>(mRenderProxy)->BoneFinalTransforms = mAnimSequence->BoneFinalTransforms;

    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();
    FRenderProxy* renderProxy = mRenderProxy;
    ENQUEUE_RENDER_COMMAND([renderThread, renderProxy]
    {
        renderProxy->UpdateRenderResource();
    });
}

void USkeletalMeshObject::createRenderProxy()
{
    //new in main thread and release in render thread
    FSkeletalMeshRenderProxyInitializer initializer;
    initializer.VertexLayout = mSkeletalMesh->GetVertexLayout();
    initializer.Vertexes = mSkeletalMesh->GetVertexes();
    initializer.Indexes = mSkeletalMesh->GetIndexes();
    initializer.VertexLayout = mSkeletalMesh->GetVertexLayout();
    initializer.Material = mMaterial->Material;
    initializer.Position = Position;
    initializer.Rotation = Rotation;
    initializer.Scale = Scale;

    mRenderProxy = new FSkeletalMeshRenderProxy(initializer);
    mRenderProxy->DebugName = Name;

    //add to scene
    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();
    FRenderProxy* renderProxy = mRenderProxy;

    ENQUEUE_RENDER_COMMAND([renderThread, renderProxy]
    {
        renderThread->AddToScene(renderProxy);
        renderProxy->CreateRenderResource();
    });

}
