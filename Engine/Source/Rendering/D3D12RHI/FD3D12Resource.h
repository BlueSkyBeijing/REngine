#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"

class FD3D12Resource
{
    friend class FD3D12RHI;

public:
    FD3D12Resource();
    virtual ~FD3D12Resource();

public:
    int32 PosInHeapCBVSRVUAV;
    int32 PosInHeapRTVDSV;

public:
    virtual void Init() {}
    virtual void UnInit() {}

    Microsoft::WRL::ComPtr <ID3D12Resource> GetDX12Resource() const
    {
        return mDX12Resource;
    }

private:
    Microsoft::WRL::ComPtr <ID3D12Resource> mDX12Resource;

};
