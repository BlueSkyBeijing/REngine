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
#include "FMaterial.h"

USkeletalMeshObject::USkeletalMeshObject() :
    mSkeletalMesh(nullptr)
{
}

USkeletalMeshObject::~USkeletalMeshObject()
{
}

void USkeletalMeshObject::Load()
{
    mSkeletalMesh = dynamic_cast<USkeletalMesh*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_SkeletalMesh, FullResourcePath));

    int32 numMat = FullMaterialPaths.size();
    for (int i = 0; i < numMat; i++)
    {
        UMaterial* mat = dynamic_cast<UMaterial*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Material, FullMaterialPaths[i]));

        mMaterials.push_back(mat);
    }

    UAnimSequence* animSequence = dynamic_cast<UAnimSequence*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Animation, FullAnimSequencePath));
    animSequence->SeSkeleton(mSkeletalMesh->GetSkeleton());
    animSequence->Load();
    mAnimSequence = new FAnimSequenceInstance(animSequence);

    //create render proxy
    createRenderProxy();
}

void USkeletalMeshObject::Unload()
{
    mSkeletalMesh = nullptr;

    mMaterials.clear();

    delete mAnimSequence;
    mAnimSequence = nullptr;
}

void USkeletalMeshObject::Update(float deltaSeconds)
{
    mAnimSequence->Update(deltaSeconds);
    for (int i = 0; i < mRenderProxy->MeshBatchs.size(); i++)
    {
        FSkeletalMeshBatch* meshBatch = dynamic_cast<FSkeletalMeshBatch*>(mRenderProxy->MeshBatchs[i]);
        meshBatch->BoneFinalTransforms = mAnimSequence->BoneFinalTransforms;
    }

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
    std::vector<FMaterial*> materials;
    for (int i = 0; i < mMaterials.size(); i++)
    {
        materials.push_back(mMaterials[i]->Material);
    }
    initializer.Materials = materials;
    initializer.Position = Position;
    initializer.Rotation = Rotation;
    initializer.Scale = Scale;
    initializer.mSections = mSkeletalMesh->GetSections();

    mRenderProxy = new FSkeletalMeshRenderProxy(initializer);
    mRenderProxy->DebugName = Name;

    //add to scene
    FRenderThread* renderThread = TSingleton<FEngine>::GetInstance().GetRenderThread();
    FRenderProxy* renderProxy = mRenderProxy;

    ENQUEUE_RENDER_COMMAND([renderThread, renderProxy]
    {
        renderProxy->CreateRenderResource();
        renderThread->AddToScene(renderProxy);
    });

}
