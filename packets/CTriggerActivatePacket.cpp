/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CTriggerActivatePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CPlayer.h"
#include "CTriggerActivatePacket.h"
#include "util/COutputBinaryStream.h"
#include "formats/rfproto.h"

void CTriggerActivatePacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt32(m_pTrigger->GetUid());
    Stream.WriteUInt32(m_pEntity->GetHandle());
    
    pPlayer->GetConnection()->AddPacket(RF_TRIGGER_ACTIVATE, Stream.str(), true);
}
