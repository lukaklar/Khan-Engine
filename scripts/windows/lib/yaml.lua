project "YAML"
	location "../../../projects/lib"
    kind "StaticLib"
	targetname "YAML"

    files {
        "../../../source/thirdparty/yaml/**.h",
		"../../../source/thirdparty/yaml/**.cpp"
    }
        
    includedirs {
        "../../../source/thirdparty/yaml/include"
    }