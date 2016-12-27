/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CJoinRequestPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CJoinRequestPacket.h"
#include "formats/rfproto.h"

CJoinRequestPacket::CJoinRequestPacket(CInputBinaryStream &Stream)
{
    m_nVersion = Stream.ReadUInt8();
    m_strName = Stream.ReadString();
    Stream.ignore(4); // unknown
    m_strPassword = Stream.ReadString();
    Stream.ignore(4); // connection_speed
    if(m_nVersion <= RF_VER_10_11)
        Stream.ignore(16); // meshes_vpp_checksum, meshes_vpp_size, motions_vpp_checksum, motions_vpp_size
    m_nTablesVppChecksum = Stream.ReadUInt32();
    m_cbTablesVpp = Stream.ReadUInt32(); // size
}
