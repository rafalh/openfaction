/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CPlayerStatsPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CPlayerStatsPacket.h"

void CPlayerStatsPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(0x07);
    Stream.WriteUInt8(m_Players.size());
    for(std::vector<CPlayer*>::const_iterator it = m_Players.begin(); it < m_Players.end(); ++it)
    {
        CPlayer * const pPlayer = *it;
        Stream.WriteUInt8(pPlayer->GetId());
        Stream.WriteUInt16(pPlayer->GetPing());
        Stream.WriteUInt8(0xFF);
        Stream.WriteUInt16(pPlayer->GetScore());
        Stream.WriteUInt8(pPlayer->GetCaptures());
        Stream.WriteUInt8(0);
    }
    Stream.WriteFloat(m_fLevelTime);
    Stream.WriteFloat(m_fTimeLimit);
    
    pPlayer->GetConnection()->AddPacket(RF_STATS, Stream.str(), false);
}
