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
		"src/*.cpp",
		"src/lzma/*.c",
		"src/aesGladman/*.cpp",
	}
	
	defines { "IRRLICHT_EXPORTS" }
	
	links { "kernel32",  "gdi32", "winmm", "opengl32", "bzip2", "jpeglib", "libpng", "zlib" }
	
	configuration "Debug"
		targetdir "../../bin/debug"
	
	configuration "Release"
		targetdir "../../bin/release"
	