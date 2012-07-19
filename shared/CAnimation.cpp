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
    rfa_header8_t Hdr;
    Stream.ReadBinary(&Hdr, sizeof(Hdr));
    
    if(Hdr.signature != RFA_SIGNATURE)
        THROW_EXCEPTION("Invalid RFA signature");
    
    if(Hdr.version != RFA_VERSION8)
        THROW_EXCEPTION("Unknown RFA version: %u", Hdr.version);
    
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
            uint16_t x = Stream.ReadUInt16();
            uint16_t y = Stream.ReadUInt16();
            uint16_t z = Stream.ReadUInt16();
            uint16_t w = Stream.ReadUInt16();
            RotKey.qRot.setValue(x, y, z, w); // FIXME
            Stream.ignore(4);
            
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

void CAnimation::Unload()
{
    m_Bones.clear();
}
