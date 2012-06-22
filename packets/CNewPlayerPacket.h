/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CNewPlayerPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CNEWPLAYERPACKET_H
#define CNEWPLAYERPACKET_H

#include "main.h"
#include "CServer.h"
#include "CPlayer.h"

class CNewPlayerPacket
{
    public:
        inline CNewPlayerPacket(CPlayer *pPlayer):
            m_pPlayer(pPlayer) {}
        
        void Send(CPlayer *pPlayer) const;
        
    private:
        CPlayer *m_pPlayer;
};

#endif // CNEWPLAYERPACKET_H
