project "OfServer"
	kind "ConsoleApp"
	language "C++"
	files {
		"*.h", "*.cpp",
		"gamemodes/*.h", "gamemodes/*.cpp",
		"../shared/*.h", "../shared/*.cpp",
		"../packets/*.h", "../packets/*.cpp",
		"../include/*.h",
	}
	includedirs {
		".",
		"../common/include",
		"../shared",
		"../packets",
		"../vendor/bullet/src",
	}
	links {
		"OF-Common",
		"BulletDynamics",
		"BulletCollision",
		"LinearMath",
	}
	defines { "OF_SERVER" }
	
	configuration { "vs*", "Debug" }
		libdirs { "../vendor/bullet/lib/vs/Debug" }
	
	configuration { "vs*", "Release" }
		libdirs { "../vendor/bullet/lib/vs/Release" }
	
	configuration "not vs*"
		libdirs { "../vendor/bullet/lib/gcc" }
	
	configuration "windows"
		links { "ws2_32" }
	
	configuration "linux"
		links { "rt", "pthread" }
	
	configuration "Debug"
		targetdir "../bin/debug"
	
	configuration "Release"
		targetdir "../bin/release"
