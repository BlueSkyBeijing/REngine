#include "PrecompiledHeader.h"

#include "FScene.h"
#include "FRenderProxy.h"
#include "FLight.h"
#include "FRHI.h"
#include "FMaterial.h"

FScene::FScene()
{
}

FScene::~FScene()
{
}

void FScene::Init()
{
    mDirectionalLight = new FDirectionalLight();
}

void FScene::UnInit()
{
    for (auto it = mStaticTranslucentMeshBatchs.begin(); it != mStaticTranslucentMeshBatchs.end(); it++)
    {
        FMeshBatch* meshBatch = *it;
        meshBatch->ReleaseRenderResource();
        delete meshBatch;
    }
    mStaticTranslucentMeshBatchs.clear();

    for (auto it = mStaticOpaqueMeshBatchs.begin(); it != mStaticOpaqueMeshBatchs.end(); it++)
    {
        FMeshBatch* meshBatch = *it;
        meshBatch->ReleaseRenderResource();
        delete meshBatch;
    }
    mStaticOpaqueMeshBatchs.clear();

    for (auto it = mDynamicOpaqueMeshBatchs.begin(); it != mDynamicOpaqueMeshBatchs.end(); it++)
    {
        FMeshBatch* meshBatch = *it;
        meshBatch->ReleaseRenderResource();
        delete meshBatch;
    }
    mDynamicOpaqueMeshBatchs.clear();

    for (auto it = mRenderProxys.begin(); it != mRenderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;
        renderProxy->ReleaseRenderResource();
        delete renderProxy;
    }
    mRenderProxys.clear();


    for (auto it = mPointLights.begin(); it != mPointLights.end(); it++)
    {
        FPointLight* pointLight = *it;
        delete pointLight;
    }
    mPointLights.clear();

    if (mDirectionalLight != nullptr)
    {
        delete mDirectionalLight;
        mDirectionalLight = nullptr;
    }
}

void FScene::AddRenderable(FRenderProxy* renderProxy)
{
    assert(renderProxy != nullptr);
    mRenderProxys.push_back(renderProxy);

    FStaticMeshRenderProxy* staticRenderProxy = dynamic_cast<FStaticMeshRenderProxy*>(renderProxy);
    if (staticRenderProxy != nullptr)
    {
        for (int i = 0; i < staticRenderProxy->MeshBatchs.size(); i++)
        {
            FMaterial* mat = staticRenderProxy->MeshBatchs[i]->Material;
            if (mat->BlendMode == BM_Translucent)
            {
                mStaticTranslucentMeshBatchs.push_back(staticRenderProxy->MeshBatchs[i]);
            }
            else
            {
                mStaticOpaqueMeshBatchs.push_back(staticRenderProxy->MeshBatchs[i]);
            }

        }
    }
    else
    {
        FSkeletalMeshRenderProxy* dynamicRenderProxy = dynamic_cast<FSkeletalMeshRenderProxy*>(renderProxy);
        assert(dynamicRenderProxy != nullptr);

        for (int i = 0; i < dynamicRenderProxy->MeshBatchs.size(); i++)
        {
            FMaterial* mat = dynamicRenderProxy->MeshBatchs[i]->Material;
            if (mat->BlendMode == BM_Translucent)
            {
                mDynamicTranslucentMeshBatchs.push_back(dynamicRenderProxy->MeshBatchs[i]);
            }
            else
            {
                mDynamicOpaqueMeshBatchs.push_back(dynamicRenderProxy->MeshBatchs[i]);
            }
        }
    }
}

void FScene::AddlLight(FLight* light)
{
    assert(light != nullptr);
    FDirectionalLight* directionalLight = dynamic_cast<FDirectionalLight*>(light);
    if (directionalLight != nullptr)
    {
        delete mDirectionalLight;
        mDirectionalLight = directionalLight;
    }

    FPointLight* pointLight = dynamic_cast<FPointLight*>(light);
    if (pointLight != nullptr)
    {
        mPointLights.push_back(pointLight);
    }

}
