#pragma once
#include "TSingleton.h"

#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include <map>
#include <string>

class FAdapter;
class FDevice;

class FDeviceManager : TSingleton<FDeviceManager>
{
public:
    FDeviceManager();
	~FDeviceManager();

    void Init();
    void UnInit();

	FDevice* GetRootDevice();

protected:

private:
    Microsoft::WRL::ComPtr <IDXGIFactory2>  mDXGIFactory;
	std::map<std::string, FDevice*> mDevices;
};
