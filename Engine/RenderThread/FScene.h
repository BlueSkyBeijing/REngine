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

    const std::vector<FMeshBatch*>& GetStaticOpaqueMeshBatchs() const
    {
        return mStaticOpaqueMeshBatchs;
    }

    const std::vector<FMeshBatch*>& GetDynamicOpaqueMeshBatchs() const
    {
        return mDynamicOpaqueMeshBatchs;
    }

    const std::vector<FMeshBatch*>& GetStaticTranslucentMeshBatchs() const
    {
        return mStaticTranslucentMeshBatchs;
    }

    const std::vector<FMeshBatch*>& GetDynamicTranslucentMeshBatchs() const
    {
        return mDynamicTranslucentMeshBatchs;
    }


    FDirectionalLight* GetDirectionalLight() const
    {
        return mDirectionalLight;
    }

    const std::vector<FPointLight*>& GetPointLights() const
    {
        return mPointLights;
    }

private:
    std::vector<FRenderProxy*> mRenderProxys;

    std::vector<FMeshBatch*> mStaticOpaqueMeshBatchs;
    std::vector<FMeshBatch*> mDynamicOpaqueMeshBatchs;

    std::vector<FMeshBatch*> mStaticTranslucentMeshBatchs;
    std::vector<FMeshBatch*> mDynamicTranslucentMeshBatchs;
    
    FDirectionalLight* mDirectionalLight;
    std::vector<FPointLight*> mPointLights;

};
