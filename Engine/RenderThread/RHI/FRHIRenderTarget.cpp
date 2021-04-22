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
    NumTarget(numTarget)
{
}

FRHIRenderTarget::~FRHIRenderTarget()
{
}

void FRHIRenderTarget::Init()
{

}

void FRHIRenderTarget::UnInit()
{

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

}

void FRHIRenderWindow::UnInit()
{

}

void FRHIRenderWindow::Present()
{
}
