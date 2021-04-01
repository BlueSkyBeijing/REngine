#pragma once

#include "UObject.h"

#include <string>

class FTexture2D;

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
    FTexture2D* Texture;

public:
    virtual void Load() override;
    virtual void Unload() override;
private:
};
