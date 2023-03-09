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