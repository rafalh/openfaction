project "Irrlicht"
	kind "SharedLib"
	
	includedirs {
		"include",
		"src",
		"../zlib",
		"../jpeglib",
		"../libpng",
	}
	
	files {
		"include/*.h",
		"src/*.cpp",
		"src/*.h",
		"src/lzma/*.c",
		"src/aesGladman/*.cpp",
	}
	
	defines { "IRRLICHT_EXPORTS" }
	
	local DXSDK_DIR = os.getenv("DXSDK_DIR")
	includedirs { DXSDK_DIR.."/Include" }
	libdirs { DXSDK_DIR.."/Lib/x86" }
	
	links { "kernel32",  "gdi32", "winmm", "opengl32", "bzip2", "jpeglib", "libpng", "zlib" }
	
	configuration "Debug"
		targetdir "../../bin/debug"
	
	configuration "Release"
		targetdir "../../bin/release"
	