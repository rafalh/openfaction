/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CItemApplyPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CITEMAPPLYPACKET_H
#define CITEMAPPLYPACKET_H

#include "CPlayer.h"
#include "CItem.h"

class CItemApplyPacket
{
    public:
        inline CItemApplyPacket(const CItem *pItem, CEntity *pEntity):
            m_pItem(pItem), m_pEntity(pEntity) {}
        void Send(CPlayer *pPlayer) const;
    
    private:
        CEntity *m_pEntity;
        const CItem *m_pItem;
};

#endif // CITEMAPPLYPACKET_H
