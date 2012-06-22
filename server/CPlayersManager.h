/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CPlayersManager.h
*  PURPOSE:     Manager for handling player list
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CPLAYERSMANAGER_H
#define CPLAYERSMANAGER_H

#include "precomp.h"
#include "CPlayer.h"

class CServer;

class CPlayersManager
{
    public:
        inline CPlayersManager(CServer *pServer):
            m_pServer(pServer), m_cPlayers(0), m_nHint(0)
        {
            for(unsigned n_Id = 0; n_Id < 0xFF; ++n_Id)
                m_Players[n_Id] = NULL;
        }
        
        inline ~CPlayersManager()
        {
            for(unsigned n_Id = 0; n_Id < 0xFF; ++n_Id)
                if(m_Players[n_Id])
                    delete m_Players[n_Id];
        }
        
        CPlayer *Get(const CSocketAddress &Addr) const;
        CPlayer *Add(const char *pName, const CSocketAddress &Addr);
        CPlayer *Enum(CPlayer *pPlayer, bool bBots = false) const;
        CPlayer *FindPlayer(const char *pStr) const;
        
        inline CPlayer *Get(unsigned nId) const
        {
            assert(nId < 0xFF);
            return m_Players[nId];
        }
        
        inline void Remove(CPlayer *pPlayer)
        {
            --m_cPlayers;
            m_Players[pPlayer->GetId()] = NULL;
            delete pPlayer;
        }
        
        inline unsigned GetCount() const
        {
            return m_cPlayers;
        }
    
    private:
        CServer *m_pServer;
        CPlayer *m_Players[0xFF];
        unsigned m_cPlayers;
        unsigned m_nHint;
};

#endif // CPLAYERSMANAGER_H
