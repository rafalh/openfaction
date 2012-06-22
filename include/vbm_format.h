/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/vbm_format.h
*  PURPOSE:     VBM format documentation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

/**
VBM (what does it stand for? maybe Volition Bit Map ?)

These files contain potentially animated images.
Each frame can be encoded with more than one resolution - each of these is called a "mipmap"
Where multiple mipmaps are available each successive image is 1/4 of the size of the previous.
ie. 1/2 height and 1/2 width.
**/

#ifndef VBM_FORMAT_H_INCLUDED
#define VBM_FORMAT_H_INCLUDED

#include <stdint.h>

#ifdef C_ASSERT
#undef C_ASSERT
#define HAS_C_ASSERT
#endif
#define C_ASSERT(e) extern void __C_ASSERT__##__LINE__(int [(e)?1:-1])

C_ASSERT(sizeof(float) == 4);

#ifndef HAS_C_ASSERT
#undef C_ASSERT
#endif

#ifdef __BIG_ENDIAN__
#error Big Endian not supported
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4200)
#endif

#pragma pack(push, 1)

#define VBM_SIGNATURE 0x6D62762E // ".vbm"

enum vbm_color_format_t
{
    VBM_CF_1555 = 0,
    VBM_CF_4444 = 1,
    VBM_CF_565  = 2,
};

struct vbm_header_t
{
    uint32_t signature;      /* VBM_SIGNATURE */
    uint32_t version;        /* sometimes 1, sometimes 2 - vbm format version perhaps? */
    uint32_t width;
    uint32_t height;
    uint32_t format;         /* see vbm_color_format_t */
    uint32_t fps;            /* frames per second */
    uint32_t frames_count;   /* number of frames */
    uint32_t mipmaps_count;  /* number of mipmaps */
};

#if 0

struct vbm_file_t
{
    vbm_header_t    vbm_header;     /* A VBM is made from a header */
    vbm_frame_t     frames[];       /* and a sequence of frames */
};

struct vbm_frame_t
{
    vbm_mipmap_t    mipmaps[];      /* A frame is made of one or more mipmaps */
};

struct vbm_mipmap_t
{
    vbm_pixel_t     pixels[];       /* Mipmaps are composed of 16bpp images in one of three formats */
};

#endif // 0

union vbm_pixel_t
{
    uint16_t    raw_pixel_data;     /* Pixels are 16 bits wide in "little endian" byte order */
    struct
    {
        uint8_t blue:   5;
        uint8_t green:  5;
        uint8_t red:    5;
        uint8_t nalpha: 1; // is it ok?
    } cf_1555;
    struct
    {
        uint8_t blue:  4;
        uint8_t green: 4;
        uint8_t red:   4;
        uint8_t alpha: 4;
    } cf_4444;
    struct
    {
        uint8_t blue:  5;
        uint8_t green: 6;
        uint8_t red:   5;
    } cf_565;
};

#pragma pack(pop)

#endif // V3D_FORMAT_H_INCLUDED
