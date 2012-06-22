/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CTeamChangePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CTeamChangePacket.h"
#include "CServer.h"

CTeamChangePacket::CTeamChangePacket(CServer *pServer, CInputBinaryStream &Stream)
{
    m_pPlayer = pServer->GetPlayersMgr()->Get(Stream.ReadUInt8());
    m_bBlueTeam = Stream.ReadUInt8() ? true : false;
}

void CTeamChangePacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_pPlayer->GetId());
    Stream.WriteUInt8(m_bBlueTeam ? 1 : 0);
    
    pPlayer->GetConnection()->AddPacket(RF_TEAM_CHANGE, Stream.str(), true);
}
