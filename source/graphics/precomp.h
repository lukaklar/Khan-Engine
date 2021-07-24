#pragma once

#include <algorithm>
#include <atomic>
#include <bitset>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef KH_GFXAPI_D3D12
#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#endif // KH_GFXAPI_D3D12

#ifdef KH_GFXAPI_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#endif // KH_GFXAPI_VULKAN

// target Windows 10 or later
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

//#define WIN32_LEAN_AND_MEAN
//#define NOGDICAPMASKS
//#define NOSYSMETRICS
//#define NOMENUS
//#define NOICONS
//#define NOSYSCOMMANDS
//#define NORASTEROPS
//#define OEMRESOURCE
//#define NOATOM
//#define NOCLIPBOARD
//#define NOCOLOR
//#define NOCTLMGR
//#define NODRAWTEXT
//#define NOKERNEL
//#define NONLS
//#define NOMEMMGR
//#define NOMETAFILE
//#define NOOPENFILE
//#define NOSCROLL
//#define NOSERVICE
//#define NOSOUND
//#define NOTEXTMETRIC
//#define NOWH
//#define NOCOMM
//#define NOKANJI
//#define NOHELP
//#define NOPROFILER
//#define NODEFERWINDOWPOS
//#define NOMCX
//#define NORPC
//#define NOPROXYSTUB
//#define NOIMAGE
//#define NOTAPE
//#define NOMINMAX
//#define STRICT
#include <Windows.h>