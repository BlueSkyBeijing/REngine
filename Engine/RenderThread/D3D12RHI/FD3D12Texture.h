#pragma once
#include "Prerequisite.h"

#include "FD3D12Resource.h"
#include "FRHITexture.h"


class FD3D12Texture : public FD3D12Resource
{
public:
    FD3D12Texture();
    virtual ~FD3D12Texture();

public:
    int32 PosInShader;
    DXGI_FORMAT Format;

public:
    virtual void Init();
    virtual void Uninit();

private:
};

class FD3D12Texture2D : public FRHITexture2D, public FD3D12Texture
{
    friend FD3D12RHI;
public:
    FD3D12Texture2D();
    virtual ~FD3D12Texture2D() override;

public:
    virtual void Init() override;
    virtual void Uninit() override;

private:
    Microsoft::WRL::ComPtr <ID3D12Resource> mTexture;
    Microsoft::WRL::ComPtr <ID3D12Resource> mTextureUploadHeap;

};
