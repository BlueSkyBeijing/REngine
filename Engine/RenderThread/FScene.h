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

    std::vector<FStaticMeshRenderProxy*>& GetStaticOpaqueRenderProxys()
    {
        return mStaticOpaqueRenderProxys;
    }

    std::vector<FSkeletalMeshRenderProxy*>& GetDynamicOpaqueRenderProxys()
    {
        return mDynamicOpaqueRenderProxys;
    }

    std::vector<FStaticMeshRenderProxy*>& GetStaticTranslucentRenderProxys()
    {
        return mStaticTranslucentRenderProxys;
    }

    std::vector<FSkeletalMeshRenderProxy*>& GetDynamicTranslucentRenderProxys()
    {
        return mDynamicTranslucentRenderProxys;
    }


    FDirectionalLight* GetDirectionalLight() const
    {
        return mDirectionalLight;
    }

private:
    std::vector<FStaticMeshRenderProxy*> mStaticOpaqueRenderProxys;
    std::vector<FSkeletalMeshRenderProxy*> mDynamicOpaqueRenderProxys;

    std::vector<FStaticMeshRenderProxy*> mStaticTranslucentRenderProxys;
    std::vector<FSkeletalMeshRenderProxy*> mDynamicTranslucentRenderProxys;
    
    FDirectionalLight* mDirectionalLight;
    std::vector<FPointLight*> mPointLights;

};
