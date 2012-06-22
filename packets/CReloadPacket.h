/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CReloadPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CRELOADPACKET_H
#define CRELOADPACKET_H

#include "main.h"
#include "CServer.h"

class CReloadPacket
{
    public:
        inline CReloadPacket(CPlayer *pPlayer):
            m_pPlayer(pPlayer) {}
        
        void Send(CPlayer *pPlayer) const;
    
    private:
        CPlayer *m_pPlayer;
};

#endif // CRELOADPACKET_H
