/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/vf_format.h
*  PURPOSE:     VF (Volition Font) format documentation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

/* Work based on e-mail from Roma Sorokin <sorok-roma@yandex.ru> */

#ifndef VF_FORMAT_H_INCLUDED
#define VF_FORMAT_H_INCLUDED

#include <stdint.h>

#ifdef __BIG_ENDIAN__
#error Big Endian not supported
#endif

#pragma pack(push, 1)

#define VF_SIGNATURE 'VFNT'

/* Note: all values are in little endian */

typedef struct _vf_char_desc_t
{
	uint32_t    spacing;
	uint32_t    width;         /* char width */
	uint32_t    offset;        /* offset in vf_file::pixels */
	uint16_t    kerning_entry;
	uint16_t    user_data;
} vf_char_desc_t;

typedef struct _vf_kern_pair_t 
{
	char    c1, c2;
	int8_t  offset;
	char    padding;  /* not sure */
} vf_kern_pair_t;

typedef struct _vf_header_t
{
	uint32_t    signature;       /* VF_SIGNATURE */
	uint32_t    version;         /* font version (1 or 0) */
	uint32_t    id2;             /* if ver >= 1; unknown */
	uint32_t    num_chars;
	uint32_t    first_ascii;     /* char ascii code offset (first_ascii+index = character) */
	uint32_t    width;           /* ??? */
	uint32_t    height;          /* char height */
	uint32_t    num_kern_pairs;
	uint32_t    kern_data_size;  /* if ver == 0; else 0 */
	uint32_t    char_data_size;  /* if ver == 0; else calculated as num_chars*sizeof(vf_font_char_t) */
	uint32_t    pixel_data_size;
} vf_header_t;

#if 0

vf_file
{
	vf_header_t header;
	vf_kern_pair_t kern_pairs[kern_data_size / sizeof(vf_kern_pair_t)]; /* kerning data */
	vf_char_desc_t chars[char_data_size / sizeof(vf_char_desc_t)]; /* character descriptors */
	char pixels[pixel_data_size]; /* raw pixels in 8-bit */
}

#endif

#pragma pack(pop)

#endif // VF_FORMAT_H_INCLUDED
