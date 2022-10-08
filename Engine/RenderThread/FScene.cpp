#include "PrecompiledHeader.h"

#include "FScene.h"
#include "FRenderProxy.h"
#include "FLight.h"
#include "FRHI.h"

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
    for (auto it = mStaticTranslucentRenderProxys.begin(); it != mStaticTranslucentRenderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;
        renderProxy->ReleaseRenderResource();
        delete renderProxy;
    }
    mStaticTranslucentRenderProxys.clear();

    for (auto it = mStaticOpaqueRenderProxys.begin(); it != mStaticOpaqueRenderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;
        renderProxy->ReleaseRenderResource();
        delete renderProxy;
    }
    mStaticOpaqueRenderProxys.clear();

    for (auto it = mDynamicOpaqueRenderProxys.begin(); it != mDynamicOpaqueRenderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;
        renderProxy->ReleaseRenderResource();
        delete renderProxy;
    }
    mDynamicOpaqueRenderProxys.clear();

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
    FStaticMeshRenderProxy* staticRenderProxy = dynamic_cast<FStaticMeshRenderProxy*>(renderProxy);
    if (staticRenderProxy != nullptr)
    {
        if (staticRenderProxy->BlendMode == BM_Translucent)
        {
            mStaticTranslucentRenderProxys.push_back(staticRenderProxy);
        }
        else
        {
            mStaticOpaqueRenderProxys.push_back(staticRenderProxy);
        }
    }
    else
    {
        FSkeletalMeshRenderProxy* dynamicRenderProxy = dynamic_cast<FSkeletalMeshRenderProxy*>(renderProxy);
        assert(dynamicRenderProxy != nullptr);

        if (dynamicRenderProxy->BlendMode == BM_Translucent)
        {
            mDynamicTranslucentRenderProxys.push_back(dynamicRenderProxy);
        }
        else
        {
            mDynamicOpaqueRenderProxys.push_back(dynamicRenderProxy);
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
