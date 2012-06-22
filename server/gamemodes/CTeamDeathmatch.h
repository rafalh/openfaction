/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/gamemodes/CTeamDeathmatch.h
*  PURPOSE:     Team DM gamemode implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CTEAMDEATHMATCH_H
#define CTEAMDEATHMATCH_H

#include "CGamemode.h"
#include "rfproto.h"

class CTeamDeathmatch: public CGamemode
{
    public:
        CTeamDeathmatch(CServer *pServer):
            CGamemode(pServer),
            m_RedScore(0), m_BlueScore(0) {}
        
        void OnPlayerKill(CPlayer *pPlayer, CPlayer *pKiller);
        void OnPlayerJoin(CPlayer *pPlayer);
        
        inline unsigned GetId() const
        {
            return RF_TEAMDM;
        }
        
        static const char *GetName()
        {
            return "teamdm";
        }
        
        inline unsigned GetRedScore()
        {
            return m_RedScore;
        }
        
        inline unsigned GetBlueScore()
        {
            return m_BlueScore;
        }
    
    private:
        unsigned m_RedScore, m_BlueScore;
};

#endif // CTEAMDEATHMATCH_H
