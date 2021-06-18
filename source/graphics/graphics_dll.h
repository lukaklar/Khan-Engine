#pragma once

#ifdef KH_DYNAMIC_LINK
	#ifdef KH_BUILD_GRAPHICS_DLL
		#define KH_GRAPHICS_DLL __declspec(dllexport)
	#else
		#define KH_GRAPHICS_DLL __declspec(dllimport)
	#endif
#else
	#define KH_GRAPHICS_DLL
#endif