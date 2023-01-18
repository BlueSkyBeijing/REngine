#include "PrecompiledHeader.h"

#include "UStaticMeshObject.h"
#include "UStaticMesh.h"
#include "UMaterial.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "FRenderProxy.h"
#include "FEngine.h"
#include "TSingleton.h"
#include "FRenderCommand.h"
#include "FResourceManager.h"
#include "FMaterial.h"

UStaticMeshObject::UStaticMeshObject() :
    mStaticMesh(nullptr)
{
}

UStaticMeshObject::~UStaticMeshObject()
{
}

void UStaticMeshObject::Load()
{
    mStaticMesh = dynamic_cast<UStaticMesh*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_StaticMesh, FullResourcePath));

    int32 numMat = int32(FullMaterialPaths.size());
    for (int i = 0; i < numMat; i++)
    {
        UMaterial* mat = dynamic_cast<UMaterial*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Material, FullMaterialPaths[i]));

        mMaterials.push_back(mat);
    }

    //create render proxy
    createRenderProxy();
}

void UStaticMeshObject::Unload()
{
    mStaticMesh = nullptr;

    mMaterials.clear();
}

void UStaticMeshObject::createRenderProxy()
{
    //new in main thread and release in render thread
    FStaticMeshRenderProxyInitializer initializer;
    initializer.VertexLayout = mStaticMesh->GetVertexLayout();
    initializer.Vertexes = mStaticMesh->GetVertexes();
    initializer.Indexes = mStaticMesh->GetIndexes();
    TArray<FMaterial*> materials;
    for (int i = 0; i < mMaterials.size(); i++)
    {
        materials.push_back(mMaterials[i]->Material);
    }
    initializer.Materials = materials;
    initializer.Position = Position;
    initializer.Rotation = Rotation;
    initializer.Scale = Scale;
    initializer.mSections = mStaticMesh->GetSections();

    mRenderProxy = new FStaticMeshRenderProxy(initializer);
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
