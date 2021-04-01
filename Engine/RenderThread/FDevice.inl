#pragma once

#include "d3dx12.h"
#include "DX12Utility.h"
#include "WindowsUtility.h"

template <typename TBufferStruct>
void FDevice::CreateConstantBuffer(FConstantBuffer<TBufferStruct>* constantBuffer)
{
    // Create object constant buffer
    CD3DX12_RESOURCE_DESC objConstantResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(CalcConstantBufferByteSize(sizeof(DirectX::XMMATRIX)));

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
    THROW_IF_FAILED(mDX12Device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &objConstantResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer->mConstantBuffer)));

    UINT8* objectConstantBufferData;
    constantBuffer->mConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&objectConstantBufferData));
    memcpy(objectConstantBufferData, &constantBuffer->BufferStruct, sizeof(constantBuffer->BufferStruct));
    constantBuffer->mConstantBuffer->Unmap(0, nullptr);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDescObject;
    cbvDescObject.BufferLocation = constantBuffer->mConstantBuffer->GetGPUVirtualAddress();
    cbvDescObject.SizeInBytes = CalcConstantBufferByteSize(sizeof(TBufferStruct));

    CD3DX12_CPU_DESCRIPTOR_HANDLE objConDescriptor(mCBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart());
    objConDescriptor = objConDescriptor.Offset(constantBuffer->Slot, mCBVSRVVUAVDescriptorSize);

    mDX12Device->CreateConstantBufferView(
        &cbvDescObject,
        objConDescriptor);

}

