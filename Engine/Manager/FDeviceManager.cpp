#include "FDeviceManager.h"
#include "FDevice.h"
#include "dxgidebug.h"

FDeviceManager::FDeviceManager()
{
}

FDeviceManager::~FDeviceManager()
{
}

void FDeviceManager::Init()
{
#if defined(DEBUG) || defined(_DEBUG) 
    //enable DX12 debug layer
    {
        Microsoft::WRL::ComPtr <ID3D12Debug> debugController;
        D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
        debugController->EnableDebugLayer();
    }
#endif

    Microsoft::WRL::ComPtr <IDXGIAdapter1> adapter;
    Microsoft::WRL::ComPtr <ID3D12Device> dx12Device;

    Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
    {
        THROW_IF_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&mDXGIFactory)));

        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
    }
    else
    {
        THROW_IF_FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory)));
    }

    //create DX12 device
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != mDXGIFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc = {};
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        HRESULT Result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dx12Device));
        if (SUCCEEDED(Result))
        {
            FDevice* device = new FDevice(dx12Device);
            device->mDXGIFactory = mDXGIFactory;
            device->Init();

            mDevices.insert(std::make_pair(std::to_string(adapterIndex), device));

            break;
        }
    }
}

void FDeviceManager::UnInit()
{
    for (auto itor = mDevices.begin(); itor != mDevices.end(); itor++)
    {
        FDevice* device = itor->second;
        device->UnInit();
        delete device;
    }

    mDevices.clear();

    mDXGIFactory.Reset();
}

FDevice* FDeviceManager::GetRootDevice()
{
    return mDevices.begin()->second;
}
