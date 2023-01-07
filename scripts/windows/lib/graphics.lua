project "Graphics"
	location "../../../projects/lib"
	kind "StaticLib"
	targetname "Graphics"

	pchheader "graphics/precomp.h"
	pchsource "../../../source/graphics/precomp.cpp"

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
		"../../../source/graphics/**.h",
		"../../../source/graphics/**.inl",
		"../../../source/graphics/**.hpp",
		"../../../source/graphics/**.cpp"
	}

	links {
		"GraphicsHAL"
	}