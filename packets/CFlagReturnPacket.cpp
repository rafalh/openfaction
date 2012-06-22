/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagReturnPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CFlagReturnPacket.h"
#include "gamemodes/CCaptureTheFlag.h"

void CFlagReturnPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_bBlueFlag ? 0 : 1);
    if(m_pPlayer)
        Stream.WriteUInt8(m_pPlayer->GetId());
    else
        Stream.WriteUInt8(0xFF); // check
    Stream.WriteUInt8(m_pCtf->GetRedScore());
    Stream.WriteUInt8(m_pCtf->GetBlueScore());
    
    pPlayer->GetConnection()->AddPacket(RF_FLAG_RETURN, Stream.str(), true);
}
