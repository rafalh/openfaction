(workspace or solution) "Open Faction"
	configurations { "Debug", "Release" }
	defines { "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_WARNINGS", "NOMINMAX", "_USE_MATH_DEFINES" }
	includedirs "include"
	language "C++" -- default
	if location then
		location "premake-build"
	end
	if characterset then
		characterset "MBCS"
	end
	
	configuration "gmake or codeblocks"
		buildoptions "-Wno-unused-variable"
	
	configuration "Debug"
		defines { "_DEBUG", "DEBUG" }
		if symbols then
			symbols "On"
		else
			flags { "Symbols" }
		end
	
	configuration "Release"
		symbols "On"
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
