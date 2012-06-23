solution "Open Faction"
	configurations { "Debug", "Release" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	includedirs "include"
	language "C++" -- default
	
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
	
	configuration "Release"
		defines { "NDEBUG" }
		flags { "OptimizeSize" }

	configuration "linux"
		defines { "LINUX" }
	
	include "vendor"
	include "client"
	include "server"
	include "vppmgr"
