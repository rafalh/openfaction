/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CGlassKillPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CGLASSKILLPACKET_H
#define CGLASSKILLPACKET_H

#include "CPlayer.h"

class CRoom;

class CGlassKillPacket
{
    public:
        inline CGlassKillPacket(CRoom *pRoom, bool bExplosion = true, const btVector3 &vPos = btVector3()):
            m_pRoom(pRoom), m_bExplosion(bExplosion), m_vPos(vPos) {}
        
        void Send(CPlayer *pPlayer) const;
    
    private:
        CRoom *m_pRoom;
        bool m_bExplosion;
        btVector3 m_vPos;
};

#endif // CGLASSKILLPACKET_H
