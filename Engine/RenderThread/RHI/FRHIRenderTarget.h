#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"


class FRHIRenderTarget : public FRHIResource
{
    friend class FRHI;

public:
    FRHIRenderTarget(uint32 width, uint32 hight,
        uint32 numTarget);
    virtual ~FRHIRenderTarget();

    virtual void Init();
    virtual void UnInit();

public:
    uint32 Width;
    uint32 Height;
    uint32 NumTarget;

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
