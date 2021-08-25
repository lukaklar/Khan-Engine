#pragma once

#include <algorithm>
#include <atomic>
#include <bitset>
#include <map>
#include <mutex>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef KH_GFXAPI_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#endif // KH_GFXAPI_VULKAN

// target Windows 10 or later
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
#include <Windows.h>