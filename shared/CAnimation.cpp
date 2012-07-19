/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CAnimation.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CAnimation.h"
#include "rfa_format.h"
#include "CException.h"
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
    
    if(Hdr.version == RFA_VERSION8)
        LoadVer8(Stream);
    else if(Hdr.version == RFA_VERSION7)
        LoadVer7(Stream);
    else
        THROW_EXCEPTION("Unknown RFA version: %u", Hdr.version);
    
    assert(Stream);
}

void CAnimation::LoadVer8(CInputBinaryStream &Stream)
{
    rfa_header8_t Hdr;
    Stream.ReadBinary(&Hdr, sizeof(Hdr));
    
    Stream.ignore(8 + Hdr.cBones * 4); // offsets
    for(unsigned i = 0; i < Hdr.cBones; ++i)
    {
        Stream.ignore(4);
        uint16_t cRotKeys = Stream.ReadUInt16();
        uint16_t cPosKeys = Stream.ReadUInt16();
        SBone Bone;
        
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
            Stream.ignore(8); // TODO: better interpolation
            
            Bone.PosKeys.push_back(PosKey);
        }
        
        m_Bones.push_back(Bone);
    }
    
    // rfa_morph_vertices_t
    Stream.ignore(Hdr.cMorphVertices * 2);
    if(Stream.tellg() % 4)
        Stream.ignore(4 - (Stream.tellg() % 4));
    
    // rfa_morph_keyframes_t::Times
    Stream.ignore(Hdr.cMorphKeyframes * 4);
    // rfa_morph_keyframes_t::Aabb
    btVector3 vAabbMin = Stream.ReadVector();
    btVector3 vAabbMax = Stream.ReadVector();
    assert(vAabbMin[0] <= vAabbMax[0] && vAabbMin[1] <= vAabbMax[1] && vAabbMin[2] <= vAabbMax[2]);
    // rfa_morph_keyframes_t::Positions
    Stream.ignore(Hdr.cMorphKeyframes * Hdr.cMorphVertices * 3);
}

void CAnimation::LoadVer7(CInputBinaryStream &Stream)
{
    rfa_header7_part1_t Hdr;
    Stream.ReadBinary(&Hdr, sizeof(Hdr));
    
    Stream.ignore(8 + Hdr.cBones * 4); // offsets
    for(unsigned i = 0; i < Hdr.cBones; ++i)
    {
        SBone Bone;
        Stream.ignore(1); // unk
        
        uint16_t cRotKeys = Stream.ReadUInt16();
        for(unsigned j = 0; j < cRotKeys; ++j)
        {
            SRotKey RotKey;
            RotKey.Time = Stream.ReadUInt32();
            float x = Stream.ReadUInt16() / 16383.0f;
            float y = Stream.ReadUInt16() / 16383.0f;
            float z = Stream.ReadUInt16() / 16383.0f;
            float w = Stream.ReadUInt16() / 16383.0f;
            RotKey.qRot.setValue(x, y, z, w);
            Stream.ignore(6 * 2 + 2); // TODO: better interpolation
            
            Bone.RotKeys.push_back(RotKey);
        }
        
        uint16_t cPosKeys = Stream.ReadUInt16();
        for(unsigned j = 0; j < cPosKeys; ++j)
        {
            SPosKey PosKey;
            PosKey.Time = Stream.ReadUInt32();
            PosKey.vPos = Stream.ReadVector();
            Stream.ignore(8); // TODO: better interpolation
            
            Bone.PosKeys.push_back(PosKey);
        }
        
        m_Bones.push_back(Bone);
    }
    
    rfa_header7_part2_t Hdr2;
    Stream.ReadBinary(&Hdr2, sizeof(Hdr2));
    
    // rfa_morph_keyframes7_t
    for(unsigned i = 0; i < Hdr2.cMorphVertices; ++i)
        Stream.ignore(4 + 12 * Hdr2.cMorphKeyframes);
}

void CAnimation::Unload()
{
    m_Bones.clear();
}
