#pragma once
#include "Prerequisite.h"

#include "FRHITexture.h"


class FRHIRenderTarget
{
    friend class FRHI;

public:
    FRHIRenderTarget(uint32 width, uint32 hight,
        uint32 numTarget);
    virtual ~FRHIRenderTarget();

    virtual void Init();
    virtual void UnInit();

    uint32 GetRenderTargetIndex() const
    {
        return mRenderTargetIndex;
    }

public:
    uint32 Width;
    uint32 Height;
    uint32 NumTarget;

    std::vector<FRHITexture2D*> RenderTargets;
    FRHITexture2D* DepthStencilTarget;

    uint32 mRenderTargetIndex;

protected:

};

class FRHIRenderWindow : public FRHIRenderTarget
{
    friend class FRHI;

public:
    FRHIRenderWindow(uint32 width, uint32 hight, uint32 numTarget);
    virtual ~FRHIRenderWindow();

    virtual void Init();
    virtual void UnInit();

    virtual void Present();

private:
};
