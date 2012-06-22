/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CPingPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef _CPINGPACKET_H
#define _CPINGPACKET_H

#include "precomp.h"
#include "rfproto.h"
#include "CPlayer.h"

class CPingPacket
{
    public:
        inline void Send(CPlayer *pPlayer)
        {
            pPlayer->GetConnection()->AddPacket(RF_PING, "", false);
        }
};

#endif // _CPINGPACKET_H
