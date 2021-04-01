#include "FScene.h"
#include "FRenderProxy.h"
#include <assert.h>

FScene::FScene()
{
}

FScene::~FScene()
{
}

void FScene::Init()
{
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
}

void FScene::AddRenderable(FRenderProxy* renderProxy)
{
    assert(renderProxy != nullptr);
    mRenderProxys.push_back(renderProxy);
}
