/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CTeamChangePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CTEAMCHANGEPACKET_H
#define CTEAMCHANGEPACKET_H

#include "CPlayer.h"
#include "CInputBinaryStream.h"

class CTeamChangePacket
{
    public:
        inline CTeamChangePacket(CPlayer *pPlayer):
            m_pPlayer(pPlayer), m_bBlueTeam(m_pPlayer->IsBlueTeam()) {}
        
        CTeamChangePacket(CServer *pServer, CInputBinaryStream &Stream);
        void Send(CPlayer *pPlayer) const;
        
        inline CPlayer *GetPlayer() const
        {
            return m_pPlayer;
        }
        
        inline bool IsBlueTeam() const
        {
            return m_bBlueTeam;
        }
        
    private:
        CPlayer *m_pPlayer;
        bool m_bBlueTeam;
};

#endif // CTEAMCHANGEPACKET_H
