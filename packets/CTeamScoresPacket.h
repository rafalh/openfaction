/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CTeamScoresPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CTEAMSCORESPACKET_H
#define CTEAMSCORESPACKET_H

#include  "gamemodes/CGamemode.h"

class CTeamScoresPacket
{
    public:
        inline CTeamScoresPacket(CGamemode *pGamemode):
            m_pGamemode(pGamemode) {}
        
        inline void Send(CPlayer *pPlayer) const
        {
            COutputBinaryStringStream Stream;
            
            Stream.WriteUInt16(m_pGamemode->GetRedScore());
            Stream.WriteUInt16(m_pGamemode->GetBlueScore());
            
            pPlayer->GetConnection()->AddPacket(RF_TEAM_SCORES, Stream.str(), true);
        }
    
    private:
        CGamemode *m_pGamemode;
};

#endif // CTEAMSCORESPACKET_H
