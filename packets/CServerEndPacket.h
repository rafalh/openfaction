/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CServerEndPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSERVERENDPACKET_H
#define CSERVERENDPACKET_H

#include "precomp.h"
#include "rfproto.h"
#include "CSocket.h"

class CServerEndPacket
{ 
    public:
        inline void Send(CPlayer *pPlayer) const
        {
            pPlayer->GetConnection()->AddPacket(RF_SERVER_STOP, "", true);
        }
};

#endif // CSERVERENDPACKET_H
