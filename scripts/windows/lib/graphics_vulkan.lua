project "Graphics"
	location "../../../projects/lib"
	kind "StaticLib"
	targetname "Graphics"

	pchheader "graphics/precomp.h"
	pchsource "../../../source/graphics/precomp.cpp"

	defines {
		"KH_GFXAPI_VULKAN",
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
		"../../../source/graphics/**.h",
		"../../../source/graphics/**.inl",
		"../../../source/graphics/**.hpp",
		"../../../source/graphics/**.cpp"
	}

	links {
		"../../../extern/VulkanSDK/Windows/Lib/vulkan-1.lib"
	}

	includedirs {
		"../../../extern/VulkanSDK/Include"
	}

	filter "configurations:Debug"
		defines {
			"VK_VALIDATE"
		}

	filter "configurations:FastDebug"
		defines {
			"VK_VALIDATE"
		}

	filter "configurations:Release"
		defines {
			"VK_VALIDATE"
		}