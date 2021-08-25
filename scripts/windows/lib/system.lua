project "System"
	location "../../../projects/lib"
	kind "StaticLib"
	targetname "System"

	pchheader "system/precomp.h"
	pchsource "../../../source/system/precomp.cpp"

	defines {
		"WIN32_LEAN_AND_MEAN",
		"NOGDICAPMASKS",
		"NOMENUS",
		"NOICONS",
		"NOSYSCOMMANDS",
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
		"../../../source/system/**.h",
		"../../../source/system/**.hpp",
		"../../../source/system/**.inl",
		"../../../source/system/**.cpp"
	}

	links {
		"Core"
	}