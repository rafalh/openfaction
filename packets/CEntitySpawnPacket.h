/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CEntitySpawnPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSPAWNPACKET_H
#define CSPAWNPACKET_H

#include "CPlayer.h"
#include "CLevel.h"
#include "CEntity.h"

class CEntitySpawnPacket
{
    public:
        inline CEntitySpawnPacket(CEntity *pEntity, CPlayer *pPlayer = NULL):
            m_pEntity(pEntity), m_pPlayer(pPlayer) {}
        
        void Send(CPlayer *pPlayer) const;
    
    private:
        CEntity *m_pEntity;
        CPlayer *m_pPlayer;
};

#endif // CSPAWNPACKET_H
