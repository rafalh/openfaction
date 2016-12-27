/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CItemSpawnPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CItemSpawnPacket.h"
#include "CServer.h"
#include "CItem.h"
#include "gamemodes/CCaptureTheFlag.h"
#include "util/utils.h"

void CItemSpawnPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    //Stream.WriteString(m_pItem->GetClass()->GetName()); //script name
    Stream.WriteString(""); //script name
    Stream.WriteUInt8(0); // unknown
    Stream.WriteUInt32(m_pItem->GetClass()->nId);
    if(m_pItem->GetBit() < 0xFFFF)
        Stream.WriteUInt32(m_pItem->GetRespawnTime() * 1000); // respawn_time
    else
        Stream.WriteUInt32(0xFFFFFFFF); // respawn_time
    Stream.WriteUInt32(m_pItem->GetCount());
    Stream.WriteUInt32(m_pItem->GetHandle()); // object handle
    Stream.WriteUInt16(m_pItem->GetBit()); // item_bit
    Stream.WriteUInt8(m_pItem->IsEnabled() ? 0x75 : 0x68); // unknown2
    
    bool bCtf = pPlayer->GetServer()->GetGamemode()->GetId() == RF_CTF;
    CCaptureTheFlag *pCtf = bCtf ? (CCaptureTheFlag*)pPlayer->GetServer()->GetGamemode() : NULL;
    if(pCtf && pCtf->IsRedFlagDropped() && !StrCmpI(m_pItem->GetClass()->strName.c_str(), "flag_red"))
        Stream.WriteVector(pCtf->GetRedFlagOldPos());
    else if(pCtf && pCtf->IsBlueFlagDropped() && !StrCmpI(m_pItem->GetClass()->strName.c_str(), "flag_blue"))
        Stream.WriteVector(pCtf->GetBlueFlagOldPos());
    else
        Stream.WriteVector(m_pItem->GetPos());
    
    Stream.WriteVector(m_pItem->GetRotMatrix().getColumn(0));
    Stream.WriteVector(m_pItem->GetRotMatrix().getColumn(1));
    Stream.WriteVector(m_pItem->GetRotMatrix().getColumn(2));
    
    pPlayer->GetConnection()->AddPacket(RF_ITEM_SPAWN, Stream.str(), true);
}
