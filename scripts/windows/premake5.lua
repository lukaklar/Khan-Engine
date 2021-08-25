architecture "x64"
language "C++"
cppdialect "C++17"
characterset "MBCS"
floatingpoint "Fast"
vectorextensions "SSE2"
startproject "Main"

configurations {
	"Debug",
	"FastDebug",
	"Release",
	"Profile",
	"Final"
}

includedirs {
	"../../source"
}

flags {
	"MultiProcessorCompile"
}

defines {
	"KH_FORCE_INLINE=__forceinline",
	"GLM_FORCE_INTRINSICS",
	"GLM_FORCE_SWIZZLE",
	"GLM_ENABLE_EXPERIMENTAL"
}

filter "system:windows"
	systemversion "latest"

filter "configurations:Debug"
	defines {
		"DEBUG",
		"_DEBUG",
		"KH_DEBUG"
	}
	optimize "Off"
	symbols "On"

filter "configurations:FastDebug"
	defines {
		"DEBUG",
		"_DEBUG",
		"KH_DEBUG"
	}
	optimize "Debug"
	symbols "On"

filter "configurations:Release"
	defines {
		"NDEBUG",
		"KH_RELEASE"
	}
	optimize "On"
	symbols "On"

filter "configurations:Profile"
	defines {
		"NDEBUG",
		"KH_PROFILE"
	}
	optimize "Speed"
	symbols "Off"

filter "configurations:Final"
	defines {
		"NDEBUG",
		"KH_FINAL"
	}
	optimize "Speed"
	symbols "Off"

workspace "khan.engine.lib.vs2019.vulkan"
	location "../../projects"
	targetdir "../../bin/lib"
	objdir "../../tmp/lib"
	staticruntime "On"

	include "lib/zlib.lua"
	include "lib/assimp.lua"
	include "lib/yaml.lua"
	include "lib/core.lua"
	include "lib/system.lua"
	include "lib/graphics_vulkan.lua"
	include "lib/data.lua"
	include "lib/engine.lua"
	include "lib/main.lua"