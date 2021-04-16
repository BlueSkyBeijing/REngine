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
    for (auto it = mRenderProxys.begin(); it != mRenderProxys.end(); it++)
    {
        FRenderProxy* renderProxy = *it;
        renderProxy->ReleaseRenderResource();
        delete renderProxy;
    }

    mRenderProxys.clear();

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
