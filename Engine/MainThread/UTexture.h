#pragma once
#include "Prerequisite.h"

#include "UObject.h"

class FRHITexture;

class UTexture : public UResource
{
public:
    UTexture();
    virtual ~UTexture() override;

private:

};

class UTexture2D : public UTexture
{
public:
    UTexture2D();
    virtual ~UTexture2D() override;

public:
    FRHITexture* Texture;

public:
    virtual void Load() override;
    virtual void Unload() override;
private:
};
