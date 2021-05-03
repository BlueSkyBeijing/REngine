#include "PrecompiledHeader.h"

#include "FRHIRenderTarget.h"
#include "FRHI.h"
#include "WindowsUtility.h"

FRHIRenderTarget::FRHIRenderTarget(
    uint32 width,
    uint32 height,
    uint32 numTarget) :
    Width(width),
    Height(height),
    NumTarget(numTarget),
    DepthStencilTarget(nullptr)
{
}

FRHIRenderTarget::~FRHIRenderTarget()
{
}

void FRHIRenderTarget::Init()
{
    RenderTargets.resize(NumTarget);
    RenderTargets.shrink_to_fit();
}

void FRHIRenderTarget::UnInit()
{
    for (int32 i = 0; i < RenderTargets.size(); i++)
    {
        delete RenderTargets[i];
    }
    RenderTargets.clear();

    if (DepthStencilTarget != nullptr)
    {
        delete DepthStencilTarget;
        DepthStencilTarget = nullptr;
    }
}

FRHIRenderWindow::FRHIRenderWindow(
    uint32 width,
    uint32 height,
    uint32 numTarget) :
    FRHIRenderTarget(width, height, numTarget)
{
}

FRHIRenderWindow::~FRHIRenderWindow()
{
}

void FRHIRenderWindow::Init()
{
    FRHIRenderTarget::Init();
}

void FRHIRenderWindow::UnInit()
{
    FRHIRenderTarget::UnInit();
}

void FRHIRenderWindow::Present()
{
}
