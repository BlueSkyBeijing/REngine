#pragma once

#include <dxgi1_6.h>
#include <wrl/client.h>
#include <vector>

class FDevice;

class FAdapter
{
public:
	FAdapter();
	~FAdapter();

private:
    Microsoft::WRL::ComPtr <IDXGIAdapter1> mDXGIAdapter;

	std::vector<FDevice*> mDevices;
};
