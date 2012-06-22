/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CEntityKillPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CKILLPLAYERPACKET_H
#define CKILLPLAYERPACKET_H

#include "CPlayer.h"
#include "CItem.h"

class CEntityKillPacket
{
    public:
        inline CEntityKillPacket(CEntity *pEntity, CPlayer *pKilled = NULL, CPlayer *pKiller = NULL, CItem *pItem = NULL):
            m_pEntity(pEntity), m_pKilled(pKilled), m_pKiller(pKiller), m_pItem(pItem) {}
        
        void Send(CPlayer *pPlayer) const;
    
    private:
        CEntity *m_pEntity;
        CPlayer *m_pKilled, *m_pKiller;
        CItem *m_pItem;
};

#endif // CKILLPLAYERPACKET_H
