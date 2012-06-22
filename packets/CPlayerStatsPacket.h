/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CPlayerStatsPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CPLAYERSTATSPACKET_H
#define CPLAYERSTATSPACKET_H

#include "precomp.h"
#include "rfproto.h"
#include "COutputBinaryStream.h"
#include "CPlayer.h"

class CPlayerStatsPacket
{
    public:
        inline CPlayerStatsPacket(float fLevelTime = 0.0f, float fTimeLeft = -1.0f):
            m_fLevelTime(fLevelTime), m_fTimeLimit(fTimeLeft) {}
        
        inline void AddPlayer(CPlayer *pPlayer)
        {
            m_Players.push_back(pPlayer);
        }
        
        void Send(CPlayer *pPlayer) const;
        
    private:
        std::vector<CPlayer*> m_Players;
        float m_fLevelTime, m_fTimeLimit;
};

#endif // CPLAYERSTATSPACKET_H
