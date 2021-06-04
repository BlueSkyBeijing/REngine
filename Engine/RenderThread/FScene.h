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

    const std::vector<FStaticMeshRenderProxy*>& GetStaticOpaqueRenderProxys() const
    {
        return mStaticOpaqueRenderProxys;
    }

    const std::vector<FSkeletalMeshRenderProxy*>& GetDynamicOpaqueRenderProxys() const
    {
        return mDynamicOpaqueRenderProxys;
    }

    const std::vector<FStaticMeshRenderProxy*>& GetStaticTranslucentRenderProxys() const
    {
        return mStaticTranslucentRenderProxys;
    }

    const std::vector<FSkeletalMeshRenderProxy*>& GetDynamicTranslucentRenderProxys() const
    {
        return mDynamicTranslucentRenderProxys;
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
    std::vector<FStaticMeshRenderProxy*> mStaticOpaqueRenderProxys;
    std::vector<FSkeletalMeshRenderProxy*> mDynamicOpaqueRenderProxys;

    std::vector<FStaticMeshRenderProxy*> mStaticTranslucentRenderProxys;
    std::vector<FSkeletalMeshRenderProxy*> mDynamicTranslucentRenderProxys;
    
    FDirectionalLight* mDirectionalLight;
    std::vector<FPointLight*> mPointLights;

};
