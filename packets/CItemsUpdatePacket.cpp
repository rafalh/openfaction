/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CItemsUpdatePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CItemsUpdatePacket.h"
#include "CItem.h"
#include "CServer.h"
#include "CLevel.h"
#include "CElementsMgr.h"

using namespace std;

void CItemsUpdatePacket::Send(CPlayer *pPlayer) const
{
    const set<CElement*> Items = pPlayer->GetServer()->GetLevel()->GetElementsMgr()->GetByType(OFET_ITEM);
    set<CElement*>::iterator it;
    string ItemsBitmap;
    
    for(it = Items.begin(); it != Items.end(); ++it)
    {
        const CItem * const pItem = (CItem*)*it;
        if(pItem->GetBit() < 0xFFFF)
        {
            unsigned i = pItem->GetBit();
            
            if(i / 8 >= ItemsBitmap.size())
                ItemsBitmap.resize(i / 8 + 1);
            
            if(pItem->IsEnabled())
                ItemsBitmap[i / 8] |= 1 << (i % 8);
        }
    }
    
    if(!ItemsBitmap.empty())
        pPlayer->GetConnection()->AddPacket(RF_ITEM_UPDATE, ItemsBitmap, false);
}
