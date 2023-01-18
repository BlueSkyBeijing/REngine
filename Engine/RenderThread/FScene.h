#pragma once
#include "Prerequisite.h"

class FScene
{
public:
    FScene();
    ~FScene();

public:
    void Init();
    void UnInit();

    void AddRenderable(FRenderProxy* renderProxy);

    void AddlLight(FLight* light);

    const TArray<FMeshBatch*>& GetStaticOpaqueMeshBatchs() const
    {
        return mStaticOpaqueMeshBatchs;
    }

    const TArray<FMeshBatch*>& GetDynamicOpaqueMeshBatchs() const
    {
        return mDynamicOpaqueMeshBatchs;
    }

    const TArray<FMeshBatch*>& GetStaticTranslucentMeshBatchs() const
    {
        return mStaticTranslucentMeshBatchs;
    }

    const TArray<FMeshBatch*>& GetDynamicTranslucentMeshBatchs() const
    {
        return mDynamicTranslucentMeshBatchs;
    }


    FDirectionalLight* GetDirectionalLight() const
    {
        return mDirectionalLight;
    }

    const TArray<FPointLight*>& GetPointLights() const
    {
        return mPointLights;
    }

private:
    TArray<FRenderProxy*> mRenderProxys;

    TArray<FMeshBatch*> mStaticOpaqueMeshBatchs;
    TArray<FMeshBatch*> mDynamicOpaqueMeshBatchs;

    TArray<FMeshBatch*> mStaticTranslucentMeshBatchs;
    TArray<FMeshBatch*> mDynamicTranslucentMeshBatchs;
    
    FDirectionalLight* mDirectionalLight;
    TArray<FPointLight*> mPointLights;

};
