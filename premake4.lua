solution "Open Faction"
	configurations { "Debug", "Release" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	includedirs "include"
	language "C++" -- default
	
	configuration "gmake or codeblocks"
		buildoptions "-Wno-unused-variable"
	
	configuration "Debug"
		defines { "_DEBUG", "DEBUG" }
		flags { "Symbols" }
	
	configuration "Release"
		defines { "NDEBUG" }
		flags { "OptimizeSize" }

	configuration "linux"
		defines { "LINUX" }
	configuration "windows"
		defines { "WIN32" }
	
	include "vendor"
	include "client"
	include "server"
	include "vppmgr"
