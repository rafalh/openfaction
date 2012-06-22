/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagsStatePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CFlagsStatePacket.h"
#include "CItem.h"
#include "CLevel.h"
#include "gamemodes/CCaptureTheFlag.h"

void CFlagsStatePacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    CItem *pItem = NULL;
    
    if(m_pCtf->GetRedFlagPlayer())
        Stream.WriteUInt8(m_pCtf->GetRedFlagPlayer()->GetId());
    else
    {
        Stream.WriteUInt8(0xFF); // no player
        Stream.WriteUInt8(m_pCtf->IsRedFlagDropped() ? 0 : 1);
        if(m_pCtf->IsRedFlagDropped())
        {
            const SItemClass *pRedFlagCls = pPlayer->GetServer()->GetGame()->GetItemsTbl()->Get("flag_red");
            pItem = pPlayer->GetServer()->GetLevel()->GetItemFromClass(pRedFlagCls);
            if(!pItem)
                return;
        }
    }
    
    if(m_pCtf->GetBlueFlagPlayer())
        Stream.WriteUInt8(m_pCtf->GetBlueFlagPlayer()->GetId());
    else
    {
        Stream.WriteUInt8(0xFF); // no player
        Stream.WriteUInt8(m_pCtf->IsBlueFlagDropped() ? 0 : 1);
        if(m_pCtf->IsBlueFlagDropped())
        {
            const SItemClass *pBlueFlagCls = pPlayer->GetServer()->GetGame()->GetItemsTbl()->Get("flag_blue");
            pItem = pPlayer->GetServer()->GetLevel()->GetItemFromClass(pBlueFlagCls);
            if(!pItem)
                return;
        }
    }
    
    if(pItem)
    {
        Stream.WriteVector(pItem->GetPos());
        Stream.WriteVector(pItem->GetRotMatrix().getColumn(0));
        Stream.WriteVector(pItem->GetRotMatrix().getColumn(1));
        Stream.WriteVector(pItem->GetRotMatrix().getColumn(2));
    }
    
    pPlayer->GetConnection()->AddPacket(RF_FLAGS_STATE, Stream.str(), true);
}
