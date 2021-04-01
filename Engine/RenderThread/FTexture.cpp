#include "FTexture.h"
#include "FDeviceManager.h"
#include "FDevice.h"

#include <string>

FTexture::FTexture()
{
}

FTexture::~FTexture()
{
}
void FTexture::Init()
{
}

void FTexture::Uninit()
{
}

FTexture2D::FTexture2D()
{
}

FTexture2D::~FTexture2D()
{
}

void FTexture2D::Init()
{
    TSingleton<FDeviceManager>::GetInstance().GetRootDevice()->CreateTexture(this);
}

void FTexture2D::Uninit()
{
}
