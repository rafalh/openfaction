/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/gamemodes/CDeathmatch.cpp
*  PURPOSE:     Deathmatch gamemode implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CDeathmatch.h"
#include "CPlayer.h"
#include "CEntity.h"
#include "CServer.h"
#include "CServerConfig.h"

void CDeathmatch::OnPlayerKill(CPlayer *pPlayer, CPlayer *pKiller)
{
    CGamemode::OnPlayerKill(pPlayer, pKiller);
    if(pKiller && pKiller->GetScore() >= (int)m_pServer->GetConf()->GetKillsLimit())
        ; // end game
}
