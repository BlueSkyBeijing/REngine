#pragma once
class FRenderProxy;

class FScene
{
public:
	FScene();
	~FScene();

    void Init();
    void UnInit();
    
    void AddRenderable(FRenderProxy* renderProxy);

    std::vector<FRenderProxy*>& GetRenderProxys()
    {
        return mRenderProxys;
    }

private:
    std::vector<FRenderProxy*> mRenderProxys;

};
