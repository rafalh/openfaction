/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/gamemodes/CTeamDeathmatch.cpp
*  PURPOSE:     Team DM gamemode implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CTeamDeathmatch.h"
#include "CPlayer.h"
#include "CServer.h"
#include "CServerConfig.h"
#include "CTeamScoresPacket.h"

void CTeamDeathmatch::OnPlayerKill(CPlayer *pPlayer, CPlayer *pKiller)
{
    if(pPlayer == pKiller)
        pPlayer->SetScore(pPlayer->GetScore() - 1);
    else if(pKiller && pKiller != pPlayer)
    {
        pKiller->SetScore(pKiller->GetScore() + 1);
        if(pKiller->IsBlueTeam())
        {
            ++m_BlueScore;
            if(m_BlueScore >= m_pServer->GetConf()->GetKillsLimit())
                CLogger::GetInst().PrintStatus("Blue Team is winning!"); // TODO: end game
        }
        else
        {
            ++m_RedScore;
            if(m_RedScore >= m_pServer->GetConf()->GetKillsLimit())
                CLogger::GetInst().PrintStatus("Red Team is winning!"); // TODO: end game
        }
    }
}

void CTeamDeathmatch::OnPlayerJoin(CPlayer *pPlayer)
{
    CTeamScoresPacket(this).Send(pPlayer);
}
