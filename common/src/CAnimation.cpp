/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CAnimation.h
*  PURPOSE:     Skeletal animation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CAnimation.h"
#include "formats/rfa_format.h"
#include "util/CException.h"
#include "util/utils.h"
#include "CAnimMgr.h"

using namespace std;

CAnimation::~CAnimation()
{
    m_pAnimMgr->Remove(this);
}

void CAnimation::Load(CInputBinaryStream &Stream)
{
    rfa_header_t Hdr;
    Stream.ReadBinary(&Hdr, sizeof(Hdr));
    
    if(Hdr.signature != RFA_SIGNATURE)
        THROW_EXCEPTION("Invalid RFA signature");
    
    if(Hdr.version != RFA_VERSION8 && Hdr.version != RFA_VERSION7)
        THROW_EXCEPTION("Unknown RFA version: %u", Hdr.version);
    
    // Offsets
    ASSERT(Stream);
    uint32_t MorphVerticesOffset = Stream.ReadUInt32();
    uint32_t MorphKeyframesOffset = Stream.ReadUInt32();
    std::vector<uint32_t> BoneOffsets(Hdr.cBones);
    for(unsigned i = 0; i < Hdr.cBones; ++i)
        BoneOffsets[i] = Stream.ReadUInt32();
    
    // Bones
    for(unsigned i = 0; i < Hdr.cBones; ++i)
    {
        SBone Bone;
        ASSERT(Stream && (uint32_t)Stream.tellg() == BoneOffsets[i]);
        
        Stream.ignore(4);
        uint16_t cRotKeys = Stream.ReadUInt16();
        uint16_t cPosKeys = Stream.ReadUInt16();
        
        for(unsigned j = 0; j < cRotKeys; ++j)
        {
            SRotKey RotKey;
            RotKey.Time = Stream.ReadUInt32();
            float x = Stream.ReadUInt16() / 16383.0f;
            float y = Stream.ReadUInt16() / 16383.0f;
            float z = Stream.ReadUInt16() / 16383.0f;
            float w = Stream.ReadUInt16() / 16383.0f;
            RotKey.qRot.setValue(x, y, z, w);
            Stream.ignore(4); // TODO: better interpolation
            
            Bone.RotKeys.push_back(RotKey);
        }
        
        for(unsigned j = 0; j < cPosKeys; ++j)
        {
            SPosKey PosKey;
            PosKey.Time = Stream.ReadUInt32();
            PosKey.vPos = Stream.ReadVector();
            Stream.ignore(2 * 12); // TODO: better interpolation
            Bone.PosKeys.push_back(PosKey);
        }
        
        m_Bones.push_back(Bone);
    }
    
    // rfa_morph_vertices_t
    if(Hdr.cMorphVertices)
    {
        ASSERT(Stream && (uint32_t)Stream.tellg() == MorphVerticesOffset);
        Stream.ignore(Hdr.cMorphVertices * 2);
    }
    
    // align to 4
    if(Stream.tellg() % 4)
        Stream.ignore(4 - (Stream.tellg() % 4));
    
    if(Hdr.cMorphKeyframes * Hdr.cMorphVertices > 0)
    {
        ASSERT(Stream && (uint32_t)Stream.tellg() == MorphKeyframesOffset);
        
        if(Hdr.version == RFA_VERSION8)
        {
            // rfa_morph_keyframes8_t::Times
            Stream.ignore(Hdr.cMorphKeyframes * 4);
            
            // rfa_morph_keyframes8_t::Aabb
            btVector3 vAabbMin = Stream.ReadVector();
            btVector3 vAabbMax = Stream.ReadVector();
            ASSERT(vAabbMin[0] <= vAabbMax[0] && vAabbMin[1] <= vAabbMax[1] && vAabbMin[2] <= vAabbMax[2]);
            
            // rfa_morph_keyframes8_t::Positions
            Stream.ignore(Hdr.cMorphKeyframes * Hdr.cMorphVertices * 3);
        }
        else // RFA_VERSION7
        {
            // rfa_morph_keyframes7_t::Positions
            Stream.ignore(Hdr.cMorphKeyframes * Hdr.cMorphVertices * 12);
        }
    }
}

void CAnimation::Unload()
{
    m_Bones.clear();
}
