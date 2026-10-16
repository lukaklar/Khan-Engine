#pragma once

#include <atomic>
#include <mutex>

#ifdef KH_GFXAPI_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#endif // KH_GFXAPI_VULKAN

#ifdef KH_GFXAPI_D3D12
#include <dxgi.h>
#include <d3d12.h>
#endif // KH_GFXAPI_D3D12

// target Windows 10 or later
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
#include <Windows.h>