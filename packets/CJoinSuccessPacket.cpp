/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CJoinSuccessPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "precomp.h"
#include "CJoinSuccessPacket.h"
#include "CServer.h"
#include "CLevel.h"
#include "CServerConfig.h"

void CJoinSuccessPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteString(pPlayer->GetServer()->GetLevelFileName());
    Stream.WriteUInt32(pPlayer->GetServer()->GetLevel()->GetChecksum());
    Stream.WriteUInt8(pPlayer->GetServer()->GetConf()->GetGameType());
    Stream.WriteBinary("\x00\x00\x00", 3);
    
    unsigned nOptions = RF_GO_DEFAULT;
    if(pPlayer->GetServer()->GetConf()->GetFallDamage())
        nOptions |= RF_GO_FALL_DAMAGE;
    if(pPlayer->GetServer()->GetConf()->GetForceRespawn())
        nOptions |= RF_GO_FORCE_RESPAWN;
    if(pPlayer->GetServer()->GetConf()->GetWeaponsStay())
        nOptions |= RF_GO_WEAPONS_STAY;
    
    Stream.WriteUInt32(nOptions);
    Stream.WriteFloat(pPlayer->GetServer()->GetLevelTime() / 1000.0f);
    Stream.WriteFloat(pPlayer->GetServer()->GetConf()->GetTimeLimit() * 60.0f);
    Stream.WriteUInt8(pPlayer->GetId());
    unsigned nFlags = RF_PF_UNKNOWN;
    if(pPlayer->IsBlueTeam())
        nFlags |= RF_PF_BLUE_TEAM;
    Stream.WriteUInt32(nFlags);
    
    pPlayer->GetConnection()->AddPacket(RF_JOIN_SUCCESS, Stream.str(), false);
}
