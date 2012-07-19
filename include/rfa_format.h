/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/rfa_format.h
*  PURPOSE:     RFA format documentation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef RFA_FORMAT_H_INCLUDED
#define RFA_FORMAT_H_INCLUDED

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

#define RFA_SIGNATURE 0x46564D56 // "VMVF"

// RF supports version 7 and 8
#define RFA_VERSION8 0x8
#define RFA_VERSION7 0x7

struct rfa_vector_t
{
    float x, y, z;
};

struct rfa_quaternion_t
{
    float x, y, z, w;
};

struct rfa_header_t
{
    uint32_t signature; // RFA_SIGNATURE
    uint32_t version; // RFA_VERSION8 / RFA_VERSION7
    float unk; // delta
    float unk2; // epsilon
    uint32_t StartTime;
    uint32_t EndTime;
    uint32_t cBones;
    uint32_t cMorphVertices;
    uint32_t cMorphKeyframes;
    uint32_t RampInTime; // ramp in * 160
    uint32_t RampOutTime; // ramp out * 160
    rfa_quaternion_t unk3;
    rfa_vector_t unk4;
};

struct rfa_vector8_t
{
    uint8_t x, y, z;
};

struct rfa_vector16_t
{
    uint16_t x, y, z;
};

struct rfa_quaternion16_t
{
    uint16_t x, y, z, w;
};

struct rfa_aabb_t
{
    rfa_vector_t vMin;
    rfa_vector_t vMax;
};

#if 0

/** File layout
    rfa_header_t;
    rfa_offsets_t;
    rfa_bone_t Bones[cBones];
    rfa_morph_vertices_t;
    // alignment to 4
    rfa_morph_keyframes_t;
**/

struct rfa_offsets_t
{
    // Offsets are from file begin
    uint32_t MorphVerticesOffset; // offset to rfa_morph_vertices_t
    uint32_t MorphKeyframesOffset; // offset to rfa_morph_keyframes_t
    uint32_t BoneOffsets[cBones]; // offsets to rfa_bone_t
};

struct rfa_bone_t
{
    float unk; // 0-255?
    uint16_t cRotKeys;
    uint16_t cPosKeys;
    rfa_rot_key_t RotKeys[cRotKeys];
    rfa_pos_key_t PosKeys[cPosKeys];
};

struct rfa_rot_key_t // size = 4+4*2+4=16
{
    uint32_t Time;
    rfa_quaternion16_t qRot;
    int8_t NextInterp; // some interpolation factors
    int8_t PrevInterp; // some interpolation factors
    int8_t unk3[2]; // always 0?
};

struct rfa_pos_key_t // size = 4+9*4=40
{
    uint32_t Time;
    rfa_vector_t vPos; // position in t=Time
    rfa_vector_t vPrevInterp; // used for interpolation before Time
    rfa_vector_t vNextInterp; // used for interpolation after Time
};

struct rfa_morph_vertices_t
{
    uint16_t VertexIndices[cMorphVertices];
};

struct rfa_morph_keyframes_t
{
    uint32_t Times[cMorphKeyframes];
    rfa_aabb_t Aabb; // exists if cMorphKeyframes*cMorphVertices > 0
    rfa_vector8_t Positions[cMorphKeyframes][cMorphVertices];
};

#endif // 0

#pragma pack(pop)

#endif // RFA_FORMAT_H_INCLUDED
