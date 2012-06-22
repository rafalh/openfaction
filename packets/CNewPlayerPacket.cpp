/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CNewPlayerPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CNewPlayerPacket.h"
#include "main.h"
#include "CServerConfig.h"

void CNewPlayerPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_pPlayer->GetId());
    Stream.WriteUInt32(pPlayer->GetServer()->GetConf()->GetMaskIp() ? 0 : pPlayer->GetAddr().GetAddress());
    Stream.WriteUInt16(pPlayer->GetServer()->GetConf()->GetMaskIp() ? 0 : pPlayer->GetAddr().GetPort());
    unsigned nFlags = RF_PF_UNKNOWN;
    if(m_pPlayer->IsBlueTeam())
        nFlags |= RF_PF_BLUE_TEAM;
    Stream.WriteUInt32(nFlags);
    Stream.WriteUInt32(10000);
    Stream.WriteString(m_pPlayer->GetName());
    
    pPlayer->GetConnection()->AddPacket(RF_NEW_PLAYER, Stream.str(), true);
}
