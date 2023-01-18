#pragma once
#include "Prerequisite.h"

#include "FRHITexture.h"


class FRHIRenderTarget
{
public:
    FRHIRenderTarget(uint32 width, uint32 hight,
        uint32 numTarget);
    virtual ~FRHIRenderTarget();

public:
    virtual void Init();
    virtual void UnInit();

    inline uint32 GetRenderTargetIndex() const
    {
        return mRenderTargetIndex;
    }

public:
    uint32 Width;
    uint32 Height;
    uint32 NumTarget;

    TArray<FRHITexture2D*> RenderTargets;
    FRHITexture2D* DepthStencilTarget;

protected:
    uint32 mRenderTargetIndex;

};

class FRHIRenderWindow : public FRHIRenderTarget
{
public:
    FRHIRenderWindow(uint32 width, uint32 hight, uint32 numTarget);
    virtual ~FRHIRenderWindow();

    virtual void Init();
    virtual void UnInit();

    virtual void Present();

private:
};
