/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CPlayersListPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CPLAYERSLISTPACKET_H
#define CPLAYERSLISTPACKET_H

#include "precomp.h"
#include "CPlayer.h"

class CPlayersListPacket
{
    public:
        inline void AddPlayer(CPlayer *pPlayer)
        {
            m_Players.push_back(pPlayer);
        }
        
        void Send(CPlayer *pPlayer) const;
        
    private:
        std::vector<CPlayer*> m_Players;
};

#endif // CPLAYERSLISTPACKET_H
