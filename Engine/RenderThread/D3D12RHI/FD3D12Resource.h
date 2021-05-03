#pragma once
#include "Prerequisite.h"

#include "FRHIResource.h"

class FD3D12ResourceInterface
{
    friend class FD3D12RHI;

public:
    FD3D12ResourceInterface();
    ~FD3D12ResourceInterface();

public:
    int32 PosInHeapCBVSRVUAV;
    int32 PosInHeapRTVDSV;

    virtual void Init() {}
    virtual void UnInit() {}

    Microsoft::WRL::ComPtr <ID3D12Resource> GetDX12Resource() const
    {
        return mDX12Resource;
    }

private:
    Microsoft::WRL::ComPtr <ID3D12Resource> mDX12Resource;

};
