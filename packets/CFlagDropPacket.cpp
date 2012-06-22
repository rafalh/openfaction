/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagDropPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CFlagDropPacket.h"
#include "CEntity.h"
#include "gamemodes/CCaptureTheFlag.h"
#include "COutputBinaryStream.h"
#include "CPlayer.h"

void CFlagDropPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_pPlayer->IsBlueTeam() ? 1 : 0);
    Stream.WriteUInt8(m_pCtf->GetRedScore());
    Stream.WriteUInt8(m_pCtf->GetBlueScore());
    Stream.WriteVector(m_pPlayer->GetEntity()->GetPos());
    
    pPlayer->GetConnection()->AddPacket(RF_FLAG_DROP, Stream.str(), true);
}
