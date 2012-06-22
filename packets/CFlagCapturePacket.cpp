/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagCapturePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CFlagCapturePacket.h"
#include "gamemodes/CCaptureTheFlag.h"

void CFlagCapturePacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_pPlayer->IsBlueTeam() ? 1 : 0);
    Stream.WriteUInt8(m_pPlayer->GetId());
    Stream.WriteUInt8(m_pCtf->GetRedScore());
    Stream.WriteUInt8(m_pCtf->GetBlueScore());
    
    pPlayer->GetConnection()->AddPacket(RF_FLAG_CAPTURE, Stream.str(), true);
}
