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

    std::vector<FStaticMeshRenderProxy*>& GetStaticRenderProxys()
    {
        return mStaticRenderProxys;
    }

    std::vector<FSkeletalMeshRenderProxy*>& GetDynamicRenderProxys()
    {
        return mDynamicRenderProxys;
    }

    FDirectionalLight* GetDirectionalLight() const
    {
        return mDirectionalLight;
    }

private:
    std::vector<FStaticMeshRenderProxy*> mStaticRenderProxys;
    std::vector<FSkeletalMeshRenderProxy*> mDynamicRenderProxys;
    FDirectionalLight* mDirectionalLight;
    std::vector<FPointLight*> mPointLights;

};
