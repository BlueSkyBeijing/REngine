#pragma once
#include "Utility.h"


class FRHIRenderTarget
{
    friend class FRHI;

public:
	FRHIRenderTarget(unsigned int width, unsigned int hight);
	virtual ~FRHIRenderTarget();

    virtual void Init();
    virtual void UnInit();

public:
    uint32 Width;
    uint32 Height;

protected:

};

class FRHIRenderWindow : public FRHIRenderTarget
{
    friend class FRHI;

public:
    FRHIRenderWindow(uint32 width, uint32 hight);
    virtual ~FRHIRenderWindow() override;

    virtual void Init() override;
    virtual void UnInit() override;

    virtual void Present();

private:
};
