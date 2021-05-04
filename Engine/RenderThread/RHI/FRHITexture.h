#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"


class FRHITexture : public FRHIResource
{
public:
    FRHITexture();
    virtual ~FRHITexture();

public:
    std::wstring FullFilePathName;

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
