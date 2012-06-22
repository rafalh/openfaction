project "OfServer"
	kind "ConsoleApp"
	language "C++"
	files {
		"*.h", "*.cpp",
		"../shared/*.h", "../shared/*.cpp",
		"../shared/tables/*.h", "../shared/tables/*.cpp",
		"../packets/*.h", "../packets/*.cpp"
	}
	includedirs {
		".",
		"../shared",
		"../shared/tables",
		"../packets",
		"../vendor/bullet/src"
	}
	links {
		"BulletDynamics",
		"BulletCollision",
		"LinearMath"
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
