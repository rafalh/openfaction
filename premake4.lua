(workspace or solution) "Open Faction"
	configurations { "Debug", "Release" }
	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"_CRT_NONSTDC_NO_WARNINGS",
		"_WINSOCK_DEPRECATED_NO_WARNINGS", -- deprecated functions are needed to support old platforms like WinXP
		"NOMINMAX",                        -- use algorithm header instead of defines from Windows
		"_USE_MATH_DEFINES",               -- needed for M_PI
	}
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
	include "common"
	include "vppmgr"
	include "tools"
	include "tests"
