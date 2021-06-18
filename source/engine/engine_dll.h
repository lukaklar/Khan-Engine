#pragma once

#ifdef KH_DYNAMIC_LINK
	#ifdef KH_BUILD_ENGINE_DLL
		#define KH_ENGINE_DLL __declspec(dllexport)
	#else
		#define KH_ENGINE_DLL __declspec(dllimport)
	#endif
#else
	#define KH_ENGINE_DLL
#endif