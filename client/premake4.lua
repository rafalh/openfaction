project "OfClient"
	kind "ConsoleApp"
	language "C++"
	files {
		"*.h", "*.cpp",
		"../shared/*.h", "../shared/*.cpp",
		"../shared/tables/*.h", "../shared/tables/*.cpp"
	}
	includedirs {
		".",
		"../shared",
		"../shared/tables",
		"../vendor/bullet/src",
		"../vendor/irrlicht/include",
		"../vendor/irrklang/include",
	}
	links {
		"BulletDynamics",
		"BulletCollision",
		"LinearMath",
		"Irrlicht",
		"irrKlang",
	}
	defines { "OF_CLIENT" }
	
	configuration { "vs*", "windows" }
		libdirs { "../vendor/irrklang/lib/Win32-visualStudio" }
	
	configuration { "not vs*", "windows" }
		libdirs { "../vendor/irrklang/lib/Win32-gcc" }
	
	configuration "windows"
		links { "ws2_32" }
	
	configuration "linux"
		links { "rt", "pthread" }
	
	configuration "Debug"
		targetdir "../bin/debug"
	
	configuration "Release"
		targetdir "../bin/release"
