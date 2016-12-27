/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/vpp_format.h
*  PURPOSE:     VPP format documentation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef VPP_FORMAT_H_INCLUDED
#define VPP_FORMAT_H_INCLUDED

#include <stdint.h>

#ifdef __BIG_ENDIAN__
#error Big Endian not supported
#endif

#pragma pack(push, 1)

#define VPP_SIGNATURE 0x51890ACE
#define VPP_ALIGNMENT 0x800 /* 2048 */
#define VPP_MAX_FILES 65536

/* Note: all values are in little endian */

typedef struct _vpp_header_t
{
    uint32_t    signature;    /* VPP_SIGNATURE */
    uint32_t    version;      /* 0x00000001 */
    uint32_t    file_count;   /* Note: maximum number of files in archive is 65536 */
    uint32_t    archive_size; /* Size of entire VPP */
} vpp_header_t;

typedef struct _vpp_entry_t
{
    char     file_name[60];
    uint32_t file_size;
} vpp_entry_t;

#pragma pack(pop)

#endif // VPP_FORMAT_H_INCLUDED
