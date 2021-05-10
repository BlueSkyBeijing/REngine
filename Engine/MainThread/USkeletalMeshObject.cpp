﻿#include "PrecompiledHeader.h"

#include "USkeletalMeshObject.h"
#include "USkeletalMesh.h"
#include "UMaterial.h"
#include "FEngine.h"
#include "FRenderThread.h"
#include "FRenderProxy.h"
#include "FEngine.h"
#include "TSingleton.h"
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
    mSkeletalMesh = new USkeletalMesh();
    mSkeletalMesh->FullFilePathName = FullResourcePath;
    mSkeletalMesh->Load();

    mMaterial = new UMaterial();
    mMaterial->Load();

    //create render proxy
    createRenderProxy();
}

void USkeletalMeshObject::Unload()
{
    mSkeletalMesh->Unload();
    delete mSkeletalMesh;
    mSkeletalMesh = nullptr;

    mMaterial->Unload();
    delete mMaterial;
    mMaterial = nullptr;
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
