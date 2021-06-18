project "Zlib"
	location "../../../projects/lib"
    kind "StaticLib"
	targetname "Zlib"

	disablewarnings {
		"4267",
		"4996"
	}

	files {
		"../../../source/thirdparty/zlib/**.h",
		"../../../source/thirdparty/zlib/**.c"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}