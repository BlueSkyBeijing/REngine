#pragma once

#include <vector>
#include <string>
#include <thread>
#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
#include <assert.h>

//-----------------------------
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#include <comdef.h>
#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include "d3dx12.h"
#include "pix3.h"
#ifdef _WIN64
// win64
#else
// win32
#endif

//-----------------------------
#elif __APPLE__

#if defined(TARGET_OS_OSX)
// macos
#elif TARGET_OS_IPHONE
// iphone
#else
// other APPLE os
#endif

//-----------------------------
#elif __ANDROID__
// android

//-----------------------------
#elif __linux__
// linux

//-----------------------------
#else
// other os
#endif