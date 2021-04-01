#pragma once
#include "Utility.h"

#define SWAPCHAIN_BUFFER_COUNT 2


class FRHIRenderTarget
{
    friend class FRHI;

public:
	FRHIRenderTarget(unsigned int width, unsigned int hight);
	~FRHIRenderTarget();

    virtual void Init();
    virtual void UnInit();

public:
    uint32 Width;
    uint32 Height;

protected:

};

class FRenderWindow : public FRHIRenderTarget
{
    friend class FRHI;

public:
    FRenderWindow(unsigned int width, unsigned int hight);
    ~FRenderWindow();

    virtual void Init() override;
    virtual void UnInit() override;

    void Present();

private:
};
