#pragma once
#include "FRHIResource.h"
#include "Utility.h"
#include <string>

class FRHITexture : public FRHIResource
{
public:
	FRHITexture();
    virtual ~FRHITexture();
public:
    std::wstring FilePathName;
    int32 Slot;

public:
    virtual void Init();
    virtual void Uninit();

private:

};

class FRHITexture2D : public FRHITexture
{
public:
    FRHITexture2D();
    virtual ~FRHITexture2D() override;


public:
    virtual void Init() override;
    virtual void Uninit() override;

private:

};
