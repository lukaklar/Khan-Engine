project "System"
	location "../../../projects/lib"
	kind "StaticLib"
	targetname "System"

	pchheader "system/precomp.h"
	pchsource "../../../source/system/precomp.cpp"

	files {
		"../../../source/system/**.h",
		"../../../source/system/**.c",
		"../../../source/system/**.hpp",
		"../../../source/system/**.inl",
		"../../../source/system/**.cpp"
	}

	links {
		"Core"
	}

	filter "files:../../../source/system/database/sqlite/sqlite3.c"
   		flags "NoPCH"