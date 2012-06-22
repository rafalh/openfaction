	project "bzip2"
	kind "StaticLib"
	
	includedirs {
		".",
	}
	
	files {
		"blocksort.c",
		"huffman.c",
		"crctable.c",
		"randtable.c",
		"compress.c",
		"decompress.c",
		"bzlib.c",
	}
	