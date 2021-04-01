#include "FRHIRenderTarget.h"
#include "FRHI.h"
#include "WindowsUtility.h"

FRHIRenderTarget::FRHIRenderTarget(
    unsigned int width,
    unsigned int height):
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

FRenderWindow::FRenderWindow(
    unsigned int width, 
    unsigned int height):
    FRHIRenderTarget(width, height),
{
}

FRenderWindow::~FRenderWindow()
{
}

void FRenderWindow::Init()
{
}

void FRenderWindow::UnInit()
{
}

void FRenderWindow::Present()
{
}
