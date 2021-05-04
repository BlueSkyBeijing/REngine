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

    void SetDirectionalLight(FDirectionalLight* light);

    std::vector<FRenderProxy*>& GetRenderProxys()
    {
        return mRenderProxys;
    }

    FDirectionalLight* GetDirectionalLight() const
    {
        return mDirectionalLight;
    }

private:
    std::vector<FRenderProxy*> mRenderProxys;
    FDirectionalLight* mDirectionalLight;
};
