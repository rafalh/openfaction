/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CPlayersListPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CPlayersListPacket.h"
#include "CServerConfig.h"
#include "CServer.h"

using namespace std;

void CPlayersListPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    for(vector<CPlayer*>::const_iterator it = m_Players.begin(); it < m_Players.end(); ++it)
    {
        CPlayer const * const pPlayer2 = *it;
        Stream.WriteUInt8(0x00);
        Stream.WriteUInt8(pPlayer2->GetId());
        Stream.WriteUInt32(0x00000008);
        Stream.WriteUInt32(pPlayer->GetServer()->GetConf()->GetMaskIp() ? 0 : pPlayer2->GetAddr().GetAddress());
        Stream.WriteUInt16(pPlayer->GetServer()->GetConf()->GetMaskIp() ? 0 : pPlayer2->GetAddr().GetPort());
        Stream.WriteString(pPlayer2->GetName());
        Stream.WriteUInt8(pPlayer2->IsBlueTeam() ? 1 : 0);
    }
    
    Stream.WriteUInt8(0x02);
    
    pPlayer->GetConnection()->AddPacket(RF_PLAYERS_LIST, Stream.str(), true);
}
