#pragma once
#include "Prerequisite.h"

#include "FRHITexture.h"


class FRHIRenderTarget : public FRHITexture2D
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
    int32 PosInHeapDSV;
    int32 PosInHeapRTSRV;
    int32 PosInHeapDSSRV;



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
