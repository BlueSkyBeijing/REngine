#include "PrecompiledHeader.h"

#include "UStaticMeshObject.h"
#include "UStaticMesh.h"
#include "UMaterial.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "FRenderProxy.h"
#include "FEngine.h"
#include "TSingleton.h"

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
    Name = "mesh01";

    mStaticMesh = new UStaticMesh();
    mStaticMesh->Load();

    mMaterial = new UMaterial();
    mMaterial->Load();

    //create render proxy
    createRenderProxy();
}

void UStaticMeshObject::Unload()
{
    mStaticMesh->Unload();
    delete mStaticMesh;
    mStaticMesh = nullptr;

    mMaterial->Unload();
    delete mMaterial;
    mMaterial = nullptr;
}

void UStaticMeshObject::createRenderProxy()
{
    //new in main thread and release in render thread
    FRenderProxyInitializer initializer;
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
    TSingleton<FEngine>::GetInstance().GetRenderThread()->AddToScene(mRenderProxy);
}
