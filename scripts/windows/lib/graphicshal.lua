project "GraphicsHAL"
	location "../../../projects/lib"
	kind "StaticLib"
	targetname "GraphicsHAL"

	pchheader "graphicshal/precomp.h"
	pchsource "../../../source/graphicshal/precomp.cpp"

	defines {
		"KH_GFXAPI_VULKAN",
		"KH_GFXAPI_D3D12",
		"WIN32_LEAN_AND_MEAN",
		"NOGDICAPMASKS",
		"NOSYSMETRICS",
		"NOMENUS",
		"NOICONS",
		"NOSYSCOMMANDS",
		"NORASTEROPS",
		"OEMRESOURCE",
		"NOATOM",
		"NOCLIPBOARD",
		"NOCOLOR",
		"NOCTLMGR",
		"NODRAWTEXT",
		"NOKERNEL",
		"NONLS",
		"NOMEMMGR",
		"NOMETAFILE",
		"NOOPENFILE",
		"NOSCROLL",
		"NOSERVICE",
		"NOSOUND",
		"NOTEXTMETRIC",
		"NOWH",
		"NOCOMM",
		"NOKANJI",
		"NOHELP",
		"NOPROFILER",
		"NODEFERWINDOWPOS",
		"NOMCX",
		"NORPC",
		"NOPROXYSTUB",
		"NOIMAGE",
		"NOTAPE",
		"NOMINMAX"
	}

	files {
		"../../../source/graphicshal/**.h",
		"../../../source/graphicshal/**.inl",
		"../../../source/graphicshal/**.hpp",
		"../../../source/graphicshal/**.cpp"
	}

	links {
		"../../../extern/VulkanSDK/Windows/Lib/vulkan-1.lib"
	}

	includedirs {
		"../../../extern/VulkanSDK/Include"
	}

	filter "configurations:Debug"
		defines {
			"VK_VALIDATE",
			"DX_DEBUG"
		}

	filter "configurations:FastDebug"
		defines {
			"VK_VALIDATE",
			"DX_DEBUG"
		}

	filter "configurations:Release"
		defines {
			"VK_VALIDATE",
			"DX_DEBUG"
		}