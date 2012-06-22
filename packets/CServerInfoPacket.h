/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CServerInfoPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef _CSERVERINFOPACKET_H
#define _CSERVERINFOPACKET_H

#include "precomp.h"
#include "rfproto.h"
#include "main.h"
#include "CSocket.h"
#include "CServer.h"

extern CSocket g_Sock;

class CServerInfoPacket
{
    public:
        void Send(CServer *pServer, const CSocketAddress &Addr) const;
};

#endif /* _CSERVERINFOPACKET_H */
