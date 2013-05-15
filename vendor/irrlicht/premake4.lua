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
	
	links { "kernel32",  "gdi32", "winmm", "opengl32", "bzip2", "jpeglib", "libpng", "zlib" }
	
	-- For __attribute__((packed)) for GCC 4.7 and newer
	configuration { "codeblocks or gmake" }
		buildoptions { "-mno-ms-bitfields" }
	
	configuration "Debug"
		targetdir "../../bin/debug"
	
	configuration "Release"
		targetdir "../../bin/release"
	