project "Main"
	location "../../../projects/lib"
	kind "WindowedApp"
	targetname "Main"

	pchheader "main/precomp.h"
	pchsource "../../../source/main/precomp.cpp"

	files {
		"../../../source/main/**.h",
		"../../../source/main/**.hpp",
		"../../../source/main/**.cpp"
	}

	links {
		"Engine"
	}