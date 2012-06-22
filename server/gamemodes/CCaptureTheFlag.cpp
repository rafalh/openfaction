/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/gamemodes/CCaptureTheFlag.cpp
*  PURPOSE:     CTF gamemode implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CCaptureTheFlag.h"
#include "CFlagsStatePacket.h"
#include "CTeamScoresPacket.h"
#include "CFlagStealPacket.h"
#include "CFlagCapturePacket.h"
#include "CFlagDropPacket.h"
#include "CFlagReturnPacket.h"
#include "CEntity.h"
#include "CServer.h"
#include "CItem.h"
#include "utils.h"
#include "CLevel.h"
#include "CLogger.h"

CCaptureTheFlag::CCaptureTheFlag(CServer *pServer):
    CGamemode(pServer),
    m_RedScore(0), m_BlueScore(0),
    m_RedFlagDropTimestamp(0), m_BlueFlagDropTimestamp(0),
    m_pRedFlagPlayer(NULL), m_pBlueFlagPlayer(NULL)
{
    m_pRedFlagCls = m_pServer->GetGame()->GetItemsTbl()->Get("flag_red");
    m_pBlueFlagCls = m_pServer->GetGame()->GetItemsTbl()->Get("flag_blue");
}

void CCaptureTheFlag::Update(unsigned nDeltaTime)
{
    /* Return flags if they are droped to long */
    if(m_RedFlagDropTimestamp && time(NULL) >= m_RedFlagDropTimestamp + 60)
        ReturnFlag(false);
    if(m_BlueFlagDropTimestamp && time(NULL) >= m_BlueFlagDropTimestamp + 60)
        ReturnFlag(true);
}

bool CCaptureTheFlag::OnItemApply(CItem *pItem, CEntity *pEntity)
{
    CPlayer *pPlayer = pEntity->GetPlayer();
    if(!pPlayer)
        return false;
    
    const char *pszItemClsName = pItem->GetClass()->strName.c_str();
    if(!StrCmpI(pszItemClsName, "base_red"))
    {
        if(!pPlayer->IsBlueTeam())
            CaptureFlag(pPlayer);
        
        return false;
    }
    else if(!StrCmpI(pszItemClsName, "base_blue"))
    {
        if(pPlayer->IsBlueTeam())
            CaptureFlag(pPlayer);
        
        return false;
    }
    else if(!StrCmpI(pszItemClsName, "flag_red"))
    {
        if(pPlayer->IsBlueTeam())
            StealFlag(pPlayer);
        else
            ReturnFlag(false, pPlayer);
        
        return false;
    }
    else if(!StrCmpI(pszItemClsName, "flag_blue"))
    {
        if(!pPlayer->IsBlueTeam())
            StealFlag(pPlayer);
        else
            ReturnFlag(true, pPlayer);
        
        return false;
    }
    else
        return true;
}

void CCaptureTheFlag::OnPlayerKill(CPlayer *pPlayer, CPlayer *pKiller)
{
    CGamemode::OnPlayerKill(pPlayer, pKiller);
    
    /* Drop flag if player has one */
    DropFlag(pPlayer);
}

void CCaptureTheFlag::OnPlayerJoin(CPlayer *pPlayer)
{
    CFlagsStatePacket(this).Send(pPlayer);
    CTeamScoresPacket(this).Send(pPlayer);
}

void CCaptureTheFlag::OnPlayerLeft(CPlayer *pPlayer)
{
    /* Drop flag if player has one */
    DropFlag(pPlayer);
}

bool CCaptureTheFlag::StealFlag(CPlayer *pPlayer)
{
    /* Update flag player */
    if(pPlayer->IsBlueTeam())
    {
        if(m_pRedFlagPlayer)
            return false;
        
        CItem *pItem = m_pServer->GetLevel()->GetItemFromClass(m_pRedFlagCls);
        if(!pItem)
            return false;
        
        m_pRedFlagPlayer = pPlayer;
        if(m_RedFlagDropTimestamp)
            m_RedFlagDropTimestamp = 0;
        else
            m_vRedFlagOldPos = pItem->GetPos();
    }
    else
    {
        if(m_pBlueFlagPlayer)
            return false;
        
        CItem *pItem = m_pServer->GetLevel()->GetItemFromClass(m_pBlueFlagCls);
        if(!pItem)
            return false;
        
        m_pBlueFlagPlayer = pPlayer;
        if(m_BlueFlagDropTimestamp)
            m_BlueFlagDropTimestamp = 0;
        else
            m_vBlueFlagOldPos = pItem->GetPos();
    }
    
    /* Send packet */
    CFlagStealPacket FlagStealPacket(this, pPlayer);
    for(CPlayer *pPlayer2 = m_pServer->GetPlayersMgr()->Enum(NULL); pPlayer2; pPlayer2 = m_pServer->GetPlayersMgr()->Enum(pPlayer2))
        FlagStealPacket.Send(pPlayer2);
    
    CLogger::GetInst().PrintStatus("%s flag has been stolen by %s!", pPlayer->IsBlueTeam() ? "Red" : "Blue", pPlayer->GetName());
    return true;
}

bool CCaptureTheFlag::CaptureFlag(CPlayer *pPlayer)
{
    if(pPlayer->IsBlueTeam())
    {
        /* Update flag player */
        if(m_pRedFlagPlayer != pPlayer || m_pBlueFlagPlayer)
            return false;
        m_pRedFlagPlayer = NULL;
        
        /* Update team score */
        ++m_BlueScore;
        
        /* Update flag position */
        CItem *pFlagItem = m_pServer->GetLevel()->GetItemFromClass(m_pRedFlagCls);
        if(pFlagItem)
            pFlagItem->SetPos(m_vRedFlagOldPos);
    }
    else
    {
        /* Update flag player */
        if(m_pBlueFlagPlayer != pPlayer || m_pRedFlagPlayer)
            return false;
        m_pBlueFlagPlayer = NULL;
        
        /* Update team score */
        ++m_RedScore;
        
        /* Update flag position */
        CItem *pFlagItem = m_pServer->GetLevel()->GetItemFromClass(m_pBlueFlagCls);
        if(pFlagItem)
            pFlagItem->SetPos(m_vBlueFlagOldPos);
    }
    
    /* Update player statistics */
    pPlayer->SetScore(pPlayer->GetScore() + 5);
    pPlayer->SetCaptures(pPlayer->GetCaptures() + 1);
    
    /* Send packet */
    CFlagCapturePacket FlagCapturePacket(this, pPlayer);
    for(CPlayer *pPlayer2 = m_pServer->GetPlayersMgr()->Enum(NULL); pPlayer2; pPlayer2 = m_pServer->GetPlayersMgr()->Enum(pPlayer2))
        FlagCapturePacket.Send(pPlayer2);
    
    CLogger::GetInst().PrintStatus("%s flag has been captured by %s!", pPlayer->IsBlueTeam() ? "Red" : "Blue", pPlayer->GetName());
    return true;
}

bool CCaptureTheFlag::DropFlag(CPlayer *pPlayer)
{
    if(m_pRedFlagPlayer == pPlayer)
    {
        /* Update flag player */
        m_pRedFlagPlayer = NULL;
        m_RedFlagDropTimestamp = time(NULL);
        
        /* Update flag position */
        CItem *pFlagItem = m_pServer->GetLevel()->GetItemFromClass(m_pRedFlagCls);
        if(pFlagItem)
            pFlagItem->SetPos(pPlayer->GetEntity()->GetPos());
    }
    else if(m_pBlueFlagPlayer == pPlayer)
    {
        /* Update flag player */
        m_pBlueFlagPlayer = NULL;
        m_BlueFlagDropTimestamp = time(NULL);
        
        /* Update flag position */
        CItem *pFlagItem = m_pServer->GetLevel()->GetItemFromClass(m_pBlueFlagCls);
        if(pFlagItem)
            pFlagItem->SetPos(pPlayer->GetEntity()->GetPos());
    }
    else
        return false;
    
    /* Send packet */
    CFlagDropPacket FlagDropPacket(this, pPlayer);
    for(CPlayer *pPlayer2 = m_pServer->GetPlayersMgr()->Enum(NULL); pPlayer2; pPlayer2 = m_pServer->GetPlayersMgr()->Enum(pPlayer2))
        FlagDropPacket.Send(pPlayer2);
    
    return true;
}

bool CCaptureTheFlag::ReturnFlag(bool bBlueFlag, CPlayer *pPlayer)
{
    /* Flag is no longer droped */
    if(bBlueFlag)
    {
        if(!m_BlueFlagDropTimestamp)
            return false;
        
        m_BlueFlagDropTimestamp = 0;
        
        /* Update flag position */
        CItem *pFlagItem = m_pServer->GetLevel()->GetItemFromClass(m_pBlueFlagCls);
        if(pFlagItem)
            pFlagItem->SetPos(m_vBlueFlagOldPos);
    }
    else
    {
        if(!m_RedFlagDropTimestamp)
            return false;
        
        m_RedFlagDropTimestamp = 0;
        
        /* Update flag position */
        CItem *pFlagItem = m_pServer->GetLevel()->GetItemFromClass(m_pRedFlagCls);
        if(pFlagItem)
            pFlagItem->SetPos(m_vRedFlagOldPos);
    }
    
    /* Update player statistics */
	if(pPlayer)
		pPlayer->SetScore(pPlayer->GetScore() + 1);
    
    /* Send packet */
    CFlagReturnPacket FlagReturnPacket(this, bBlueFlag, pPlayer);
    for(CPlayer *pPlayer2 = m_pServer->GetPlayersMgr()->Enum(NULL); pPlayer2; pPlayer2 = m_pServer->GetPlayersMgr()->Enum(pPlayer2))
        FlagReturnPacket.Send(pPlayer2);
    
    CLogger::GetInst().PrintStatus("%s flag returned to base!", bBlueFlag ? "Blue" : "Red");
    return true;
}
