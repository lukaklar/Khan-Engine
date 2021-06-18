project "Core"
	location "../../../projects/lib"
	kind "StaticLib"
	targetname "Core"

	pchheader "core/precomp.h"
	pchsource "../../../source/core/precomp.cpp"

	files {
		"../../../source/core/**.h",
		"../../../source/core/**.hpp",
		"../../../source/core/**.cpp"
	}