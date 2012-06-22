/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CPlayersManager.cpp
*  PURPOSE:     Manager for handling player list
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CPlayersManager.h"
#include "CPlayer.h"
#include "CServer.h"

CPlayer *CPlayersManager::Get(const CSocketAddress &Addr) const
{
    for(unsigned nId = 0; nId < 0xFF; ++nId)
    {
        if(m_Players[nId] && m_Players[nId]->GetAddr() == Addr)
            return m_Players[nId];
    }
    
    return NULL;
}

CPlayer *CPlayersManager::Add(const char *pName, const CSocketAddress &Addr)
{
    assert(m_cPlayers < 0xFF);
    
    /* Find free ID */
    while(m_Players[m_nHint])
        m_nHint = (m_nHint + 1) % 0xFF;
    
    /* Add new player */
    CPlayer * const pPlayer = new CPlayer(m_pServer, pName, m_nHint, Addr);
    m_Players[m_nHint] = pPlayer;
    ++m_cPlayers;
    
    /* Update hint */
    m_nHint = (m_nHint + 1) % 0xFF;
    
    return pPlayer;
}

CPlayer *CPlayersManager::Enum(CPlayer *pPlayer, bool bBots) const
{
    unsigned nId = pPlayer ? pPlayer->GetId() + 1 : 0;
    
    while(nId < 0xFF)
    {
        if(m_Players[nId] && (bBots || m_Players[nId]->GetAddr().GetAddress() != 0))
            return m_Players[nId];
        
        ++nId;
    }
    
    return NULL;
}

CPlayer *CPlayersManager::FindPlayer(const char *pStr) const
{
    if(!pStr[0])
        return NULL;
    
    for(unsigned nId = 0; nId < 0xFF; ++nId)
        if(m_Players[nId] && strstr(m_Players[nId]->GetName(), pStr))
            return m_Players[nId];
    
    return NULL;
}
