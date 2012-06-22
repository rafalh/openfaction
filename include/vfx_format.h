/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/vfx_format.h
*  PURPOSE:     VFX format documentation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef VFX_FORMAT_H_INCLUDED
#define VFX_FORMAT_H_INCLUDED

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

#define VFX_SIGNATURE 0x58465356 // "VSFX"
#define VFX_VERSION 0x40006

struct vfx_header_t
{
    uint32_t signature;
    uint32_t version; // minimal supported version is 0x30000
    uint32_t unknown[30];
};

struct vfx_section_t
{
    uint32_t type;
    uint32_t size; // Note: size field is included
};

enum vfx_sect_type_t
{
    // Note: there is no ending section
    VFX_SFXO = 0x4F584653,
    VFX_MATL = 0x4C54414D,
};

#if 0

vfx_sfxo_t
{
    char name[]; // "* Scene Root"
    char unknown[0x23A]; // can be more
}

vfx_matl_t
{
    uint32_t unk_type; // 0 - 2
    uint32_t unk1; // 0xF
    uint8_t unk_bool; // 1
    char filename[]; // .tga
    uint32_t unk2;
    float unk2;
    uint32_t unk3;
    // texture 2 if unk_type == 1
    float unk4[3];
    char unk5[];
    uint32_t cunk6;
    
    char unknown4[];
}

#endif // 0

#pragma pack(pop)

#endif // VFX_FORMAT_H_INCLUDED
