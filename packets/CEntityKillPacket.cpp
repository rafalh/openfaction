/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CEntityKillPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CEntityKillPacket.h"
#include "CEntity.h"
#include "formats/rfproto.h"

using namespace std;

void CEntityKillPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt32(m_pEntity->GetHandle());
    Stream.WriteUInt32(0xC47A0000); // unknown2
    Stream.WriteUInt8(m_pKiller ? m_pKiller->GetId() : 0xFF);
    Stream.WriteUInt8(m_pKilled ? m_pKilled->GetId() : 0xFF);
    Stream.WriteUInt16(0x0005); // animation
    unsigned nFlags = m_pItem ? RF_KF_ITEM : 0;
    Stream.WriteUInt8(nFlags);
    if(nFlags & (RF_KF_ITEM|RF_KF_ITEM_UNKNOWN))
    {
        Stream.WriteUInt16(0x0000); // unknown3
        Stream.WriteUInt32(m_pItem->GetClass()->nId);
        Stream.WriteBinary("\xFF\xFF\xFF\xFF\x80\x00\x00\x00", 8); // unknown4
        Stream.WriteUInt32(m_pItem->GetHandle());
        Stream.WriteVector(m_pItem->GetPos());
        
        Stream.WriteVector(m_pItem->GetRotMatrix().getColumn(0));
        Stream.WriteVector(m_pItem->GetRotMatrix().getColumn(1));
        Stream.WriteVector(m_pItem->GetRotMatrix().getColumn(2));
    }
    
    pPlayer->GetConnection()->AddPacket(RF_ENTITY_KILL, Stream.str(), true);
}
