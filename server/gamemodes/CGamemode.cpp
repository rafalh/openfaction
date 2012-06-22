/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/gamemodes/CGamemode.cpp
*  PURPOSE:     Base class for gamemodes
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CGamemode.h"
#include "CEntity.h"
#include "CPlayer.h"
#include "rfproto.h"
#include "CServer.h"

#include "CDeathmatch.h"
#include "CTeamDeathmatch.h"
#include "CCaptureTheFlag.h"

void CGamemode::OnEntityKill(CEntity *pEntity, SDamageInfo &DmgInfo)
{
    CPlayer *pPlayer = pEntity->GetPlayer();
    if(!pPlayer)
        return;
    
    CPlayer *pKillerPlayer = NULL;
    if(DmgInfo.pResponsibleObj && DmgInfo.pResponsibleObj->GetType() == OFET_ENTITY)
    {
        CEntity *pKillerEntity = (CEntity*)DmgInfo.pResponsibleObj;
        pKillerPlayer = pKillerEntity->GetPlayer();
    }
        
    OnPlayerKill(pPlayer, pKillerPlayer);
}

void CGamemode::OnPlayerKill(CPlayer *pPlayer, CPlayer *pKiller)
{
    if(pPlayer == pKiller)
        pPlayer->SetScore(pPlayer->GetScore() - 1);
    else if(pKiller)
        pKiller->SetScore(pKiller->GetScore() + 1);
}

CGamemode *CGamemode::Create(CServer *pServer, unsigned Id)
{
    if(Id == RF_DM)
        return new CDeathmatch(pServer);
    else if(Id == RF_TEAMDM)
        return new CTeamDeathmatch(pServer);
    else if(Id == RF_CTF)
        return new CCaptureTheFlag(pServer);
    else
        assert(false);
}
