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

UStaticMeshObject::UStaticMeshObject() :
    mMaterial(nullptr),
    mStaticMesh(nullptr)
{
}

UStaticMeshObject::~UStaticMeshObject()
{
}

void UStaticMeshObject::Load()
{
    mStaticMesh = dynamic_cast<UStaticMesh*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_StaticMesh, FullResourcePath));

    mMaterial = dynamic_cast<UMaterial*>(TSingleton<FResourceManager>::GetInstance().GetOrCreate(EResourceType::RT_Material, FullMaterialPath));

    //create render proxy
    createRenderProxy();
}

void UStaticMeshObject::Unload()
{
    mStaticMesh = nullptr;

    mMaterial = nullptr;
}

void UStaticMeshObject::createRenderProxy()
{
    //new in main thread and release in render thread
    FStaticMeshRenderProxyInitializer initializer;
    initializer.VertexLayout = mStaticMesh->GetVertexLayout();
    initializer.Vertexes = mStaticMesh->GetVertexes();
    initializer.Indexes = mStaticMesh->GetIndexes();
    initializer.VertexLayout = mStaticMesh->GetVertexLayout();
    initializer.Material = mMaterial->Material;
    initializer.Position = Position;
    initializer.Rotation = Rotation;
    initializer.Scale = Scale;

    mRenderProxy = new FStaticMeshRenderProxy(initializer);
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
