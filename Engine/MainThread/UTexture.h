#pragma once

#include "UObject.h"

#include <string>

class FRHITexture2D;

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
    FRHITexture2D* Texture;

public:
    virtual void Load() override;
    virtual void Unload() override;
private:
};
