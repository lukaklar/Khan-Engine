project "Engine"
	location "../../../projects/lib"
	kind "StaticLib"
	targetname "Engine"

	pchheader "engine/precomp.h"
	pchsource "../../../source/engine/precomp.cpp"

	files {
		"../../../source/engine/**.h",
		"../../../source/engine/**.hpp",
		"../../../source/engine/**.cpp"
	}

	links {
		"Core",
		"System",
		"Graphics"
	}