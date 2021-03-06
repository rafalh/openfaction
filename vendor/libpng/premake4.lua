project "libpng"
	kind "StaticLib"
	targetdir "lib"
	targetprefix ""
	includedirs { "../zlib" }
	files {
		"png.c",
		"pngerror.c",
		"pngget.c",
		"pngmem.c",
		"pngpread.c",
		"pngread.c",
		"pngrio.c",
		"pngrtran.c",
		"pngrutil.c",
		"pngset.c",
		"pngtrans.c",
		"pngwio.c",
		"pngwrite.c",
		"pngwtran.c",
		"pngwutil.c",
		"*.h",
	}
	
	configuration "Debug"
		defines { "PNG_DEBUG=1" }
