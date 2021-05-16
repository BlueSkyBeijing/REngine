#include "PrecompiledHeader.h"

#include "FScene.h"
#include "FRenderProxy.h"
#include "FLight.h"

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
    for (auto it = mStaticRenderProxys.begin(); it != mStaticRenderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;
        renderProxy->ReleaseRenderResource();
        delete renderProxy;
    }
    mStaticRenderProxys.clear();

    for (auto it = mDynamicRenderProxys.begin(); it != mDynamicRenderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;
        renderProxy->ReleaseRenderResource();
        delete renderProxy;
    }
    mDynamicRenderProxys.clear();

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
        mStaticRenderProxys.push_back(staticRenderProxy);
    }
    else
    {
        FSkeletalMeshRenderProxy* dynamicRenderProxy = dynamic_cast<FSkeletalMeshRenderProxy*>(renderProxy);
        assert(dynamicRenderProxy != nullptr);
        mDynamicRenderProxys.push_back(dynamicRenderProxy);
    }
}

void FScene::SetDirectionalLight(FDirectionalLight* light)
{
    assert(light != nullptr);
    if (mDirectionalLight != nullptr)
    {
        delete mDirectionalLight;
    }
    mDirectionalLight = light;
}
