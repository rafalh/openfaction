/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CClutterKillPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CClutterKillPacket.h"
#include "rfproto.h"

void CClutterKillPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt32(m_pClutter->GetUid());
    Stream.WriteUInt32(1); // unknown
    
    pPlayer->GetConnection()->AddPacket(RF_CLUTTER_KILL, Stream.str(), true);
}
