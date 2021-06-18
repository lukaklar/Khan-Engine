#pragma once

#ifdef KH_DYNAMIC_LINK
#	ifdef KH_BUILD_CORE_DLL
#		define KH_CORE_DLL __declspec(dllexport)
#	else
#		define KH_CORE_DLL __declspec(dllimport)
#	endif
#else
#	define KH_CORE_DLL
#endif