#pragma once
#include "FRenderResource.h"
#include "Utility.h"
#include <string>

class FTexture : public FRenderResource
{
public:
	FTexture();
    virtual ~FTexture();
public:
    std::wstring FilePathName;
    int32 Slot;

public:
    virtual void Init();
    virtual void Uninit();



private:

};

class FTexture2D : public FTexture
{
public:
    FTexture2D();
    virtual ~FTexture2D() override;


public:
    virtual void Init() override;
    virtual void Uninit() override;

private:

};
