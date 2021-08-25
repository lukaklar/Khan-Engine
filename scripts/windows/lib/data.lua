project "Data"
	location "../../../projects/lib"
	kind "StaticLib"
	targetname "Data"

	pchheader "data/precomp.h"
	pchsource "../../../source/data/precomp.cpp"

	defines {
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

	includedirs {
		"../../../source/thirdparty/assimp",
        "../../../source/thirdparty/assimp/include",
		"../../../source/thirdparty/assimp/code",
		"../../../source/thirdparty/assimp/contrib"
	}

	files {
		"../../../source/data/**.h",
		"../../../source/data/**.c",
		"../../../source/data/**.hpp",
		"../../../source/data/**.inl",
		"../../../source/data/**.cpp"
	}

	links {
		"Assimp",
		"YAML",
		"Core",
		"System",
		"Graphics"
	}

	filter "files:../../../source/data/database/sqlite/sqlite3.c"
   		flags "NoPCH"