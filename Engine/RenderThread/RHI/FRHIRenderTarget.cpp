#include "FRHIRenderTarget.h"
#include "FRHI.h"
#include "WindowsUtility.h"

FRHIRenderTarget::FRHIRenderTarget(
    uint32 width,
    uint32 height):
    Width(width),
    Height(height)
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
    uint32 height):
    FRHIRenderTarget(width, height)
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
