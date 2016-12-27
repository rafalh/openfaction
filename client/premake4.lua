project "OfClient"
	kind "WindowedApp"
	language "C++"
	files {
		"*.h", "*.cpp",
		"camera/*.h", "camera/*.cpp",
		"irr/*.h", "irr/*.cpp",
		"../shared/*.h", "../shared/*.cpp",
		"../include/*.h",
	}
	includedirs {
		".",
		"../shared",
		"../common/include",
		"../vendor/bullet/src",
		"../vendor/irrlicht/include",
		"../vendor/irrklang/include",
		"../vendor/freetype/include",
	}
	links {
		"OF-Common",
		"BulletDynamics",
		"BulletCollision",
		"LinearMath",
		"Irrlicht",
		"irrKlang",
		"FreeType",
	}
	defines { "OF_CLIENT" }
	
	configuration { "vs*", "windows" }
		libdirs { "../vendor/irrklang/lib/Win32-visualStudio" }
	
	configuration { "not vs*", "windows" }
		libdirs { "../vendor/irrklang/lib/Win32-gcc" }
	
	configuration "windows"
		flags { "WinMain" }
		links { "ws2_32" }
	
	configuration "linux"
		links { "rt", "pthread" }
	
	configuration "Debug"
		targetdir "../bin/debug"
	
	configuration "Release"
		targetdir "../bin/release"
