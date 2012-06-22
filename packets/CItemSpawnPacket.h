/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CItemSpawnPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CITEMSPAWNPACKET_H
#define CITEMSPAWNPACKET_H

#include "CLevel.h"

class CPlayer;

class CItemSpawnPacket
{
    public:
        inline CItemSpawnPacket(const CItem *pItem):
            m_pItem(pItem) {}
        
        void Send(CPlayer *pPlayer) const;
        
    private:
        const CItem * const m_pItem;
};

#endif // CITEMSPAWNPACKET_H
