#pragma once
#include "FD3D12Resource.h"
#include "FRHITexture.h"

#include "Utility.h"
#include <string>

class FD3D12Texture : public FD3D12Resource
{
public:
	FD3D12Texture();
    virtual ~FD3D12Texture();
public:
    std::wstring FilePathName;
    int32 Slot;

public:
    virtual void Init();
    virtual void Uninit();

private:

};

class FD3D12Texture2D : public FRHITexture2D
{
public:
    FD3D12Texture2D();
    virtual ~FD3D12Texture2D() override;


public:
    virtual void Init() override;
    virtual void Uninit() override;

private:

};
