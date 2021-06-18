#pragma once

#ifdef KH_DYNAMIC_LINK
	#ifdef KH_BUILD_SYSTEM_DLL
		#define KH_SYSTEM_DLL __declspec(dllexport)
	#else
		#define KH_SYSTEM_DLL __declspec(dllimport)
	#endif
#else
	#define KH_SYSTEM_DLL
#endif