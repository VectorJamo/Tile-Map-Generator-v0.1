workspace "TileMapGeneratorV10"
	configurations {
	"Debug", "Release"
	}
	platforms {
		"Win32", "Win64"
	}

	filter {"platforms:Win32"}
		system "Windows"
		architecture "x86"

	filter {"platforms:Win64"}
		system "Windows"
		architecture "x86_64"

project "TileMapGenerator"
	location "TileMapGenerator"
	
	kind "ConsoleApp"
	language "C++"
	
	files {
		"TileMapGenerator/src/**.h",
		"TileMapGenerator/src/**.cpp",
	}
	
	includedirs {
		"Dependencies/SDL3/include"
	}
	
	libdirs {
		"Dependencies/SDL3/lib/x86"
	}
	
	links {
		"SDL3", "SDL3_image"
	}
	
	targetdir "bin/%{prj.name}/%{cfg.buildcfg}-%{cfg.platform}/"