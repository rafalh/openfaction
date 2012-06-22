/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CTriggerActivatePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CTRIGGERACTIVATEPACKET_H
#define CTRIGGERACTIVATEPACKET_H

#include "CTrigger.h"
#include "CEntity.h"

class CTriggerActivatePacket
{
    public:
        inline CTriggerActivatePacket(CTrigger *pTrigger, CEntity *pEntity):
            m_pTrigger(pTrigger), m_pEntity(pEntity) {}
        
        void Send(CPlayer *pPlayer) const;
    
    private:
        CTrigger *m_pTrigger;
        CEntity *m_pEntity;
};

#endif // CTRIGGERACTIVATEPACKET_H
