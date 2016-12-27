/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagStealPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CFlagStealPacket.h"
#include "gamemodes/CCaptureTheFlag.h"
#include "formats/rfproto.h"

void CFlagStealPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_pPlayer->GetId());
    Stream.WriteUInt8(m_pCtf->GetRedScore());
    Stream.WriteUInt8(m_pCtf->GetBlueScore());
    
    pPlayer->GetConnection()->AddPacket(RF_FLAG_STEAL, Stream.str(), true);
}
