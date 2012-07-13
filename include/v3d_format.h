/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/v3d_format.h
*  PURPOSE:     V3D format specification (v3m/v3c)
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef V3D_FORMAT_H_INCLUDED
#define V3D_FORMAT_H_INCLUDED

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

/**
V3M and V3D has both the same format (called V3D).
V3M is used for static meshes without bones.
V3C is used for meshes with bones.
**/
#define V3M_SIGNATURE 0x52463344 // RF3D
#define V3C_SIGNATURE 0x5246434D // RFCM
#define V3D_VERSION 0x40000

struct v3d_header_t
{
    uint32_t signature; /* V3M_SIGNATURE */
    uint32_t version;
    uint32_t submeshes_count;
    uint32_t unknown[6];
    uint32_t colspheres_count;
    /* sections */
};

struct v3d_section_t
{
    uint32_t type;
    uint32_t size; // unused for V3M_SUBMESH
};

enum v3d_section_type_t
{
    V3D_END       = 0x00000000,
    V3D_SUBMESH   = 0x5355424D, // 'SUBM' see v3d_submesh_t
    V3D_COLSPHERE = 0x43535048, // 'CSPH' see v3d_col_sphere_t
    V3D_BONE      = 0x424F4E45, // 'BONE', see v3d_bones_t
};

struct v3d_batch_info_t // batch info?
{
    uint16_t vertices_count;
    uint16_t triangles_count;
    // some size values are with alignment
    uint16_t positions_size;
    uint16_t indices_size;
    uint16_t unknown_size;
    uint16_t bone_links_size;
    uint16_t tex_coords_size;
    uint32_t unknown3;
};

struct v3d_material_t
{
    char filename[32];
    float unknown[4];
    char filename2[32];
    uint32_t flags;
};

// Alignment in data section
#define V3D_ALIGNMENT 0x10

#if 0

v3d_batch_header_t // size: 56 (0x38)
{ // this is not used by RF - only read and then over-written
    char unknown[56];
  /*char unknown[40];
    uint16_t vertices_count; // see v3d_batch_info_t
    uint16_t triangles_count; // see v3d_batch_info_t
    uint16_t positions_size; // see v3d_batch_info_t
    uint16_t unknown2;
    uint16_t tex_coords_size; // see v3d_batch_info_t
    char unknown2[6];*/
}

v3d_triangle_t
{
    uint16_t indices[3];
    uint16_t unknown; // 0x20 (flags or padding?)
}

v3d_vertex_bones_t
{
    // One vertex can be linked maximaly to 4 bones
    uint8_t weights[4]; // in range 0-255, 0 if slot is unused
    uint8_t bones[4]; // bone indexes, 0xFF if slot is unused
}

v3d_batch_data_t
{
    float positions[v3d_batch_info_t::vertices_count * 3];
    // padding to 0x10 (to data section begin)
    float normals[v3d_batch_info_t::vertices_count * 3];
    // padding to 0x10 (to data section begin)
    float tex_coords[v3d_batch_info_t::vertices_count * 2];
    // padding to 0x10 (to data section begin)
    v3d_triangle_t triangles[v3d_batch_info_t::triangles_count];
    // padding to 0x10 (to data section begin)
    if(v3d_submesh_lod_t::flags & 0x20)
    {
        float unknown_planes[triangles_count * 4];
        // padding to 0x10 (to data section begin)
    }
    char unknown[v3d_batch_info_t::unknown_size];
    // padding to 0x10 (to data section begin)
    if(v3d_batch_info_t::bone_links_size)
    {
        v3d_vertex_bones_t bone_links[v3d_batch_info_t::vertices_count];
        // padding to 0x10 (to data section begin)
    }
    if(v3d_submesh_lod_t::flags & 0x1)
    {
        float [v3d_submesh_lod_t::vertices_count * 2];
        // padding to 0x10 (to data section begin)
    }
}

v3d_model_data_t
{
    v3d_batch_header_t batch_headers[v3d_submesh_lod_t::batches_count];
    // padding to 0x10 (to data section begin)
    v3d_batch_data_t batch_data[v3d_submesh_lod_t::batches_count];
}

v3d_texture_t
{
    uint8_t id;
    char filename[];
}

v3d_submesh_lod_t
{
    uint32_t flags;
    uint32_t vertices; // not sure
    uint16_t batches_count;
    uint32_t data_size;
    char data[data_size]; // see v3d_model_data_t
    int32_t unknown; // -1, sometimes 0
    v3d_batch_info_t batch_info[batches_count];
    uint32_t unknown2; // 0, 1
    uint32_t textures_count;
    v3d_texture_t textures[textures_count];
}

v3d_submesh_t
{
    uint32_t sect_sig; // 'SUBM'
    uint32_t sect_size; // 0 for SUBM (it has to be processed to find end)
    char name[24];
    char unknown[24]; // "None"?
    uint32_t version; // 7 (submesh ver?) values < 7 doesnt work
    uint32_t lod_models_count; // 1 - 3
    float lod_distances[lod_models_count]; // 0.0, 10.0
    float center_x, center_y, center_z; // bounding sphere position
    float radius; // bounding sphere radius
    float aabb_x1, aabb_y1, aabb_z1; // axis aligned bounding box minima
    float aabb_x2, aabb_y2, aabb_z2; // axis aligned bounding box maxima
    v3d_submesh_lod_t lod_models[lod_models_count];
    uint32_t materials_count;
    v3d_material_t materials[materials_count];
    uint32_t unknown3;
    struct
    {
        char unknown[24];
        float unknown2;
    } unknown4[unknown3];
}

v3d_col_sphere_t
{
    char name[24];
    int32_t bone; // bone index or -1
    float x, y, x; // position relative to bone
    float r; // radius
}

v3d_bone_t
{
    char name[24];
    float rot[4]; // quaternion
    float pos[3]; // bone to model translation
    int32_t parent; // bone index (root has -1)
}

v3d_bones_t
{
    uint32_t bones_count;
    v3d_bone_t bones[bones_count];
}

#endif // 0

#pragma pack(pop)

#endif // V3D_FORMAT_H_INCLUDED
