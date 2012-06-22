/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CJoinFailedPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CJOINFAILEDPACKET_H
#define CJOINFAILEDPACKET_H

#include "rfproto.h"
#include "CPlayer.h"

class CServer;

class CJoinFailedPacket
{
    public:
        inline CJoinFailedPacket(rfJoinFailedReason nReason):
            m_nReason(nReason) {}
        
        inline void Send(CServer *pServer, const CSocketAddress &Addr)
        {
            COutputBinaryStringStream Stream;
            
            Stream.WriteUInt8(RF_GAME);
            Stream.WriteUInt8(RF_JOIN_FAILED);
            Stream.WriteUInt16(1);
            Stream.WriteUInt8(m_nReason);
            
            pServer->GetSocket().SendTo(Stream.str(), Addr);
        }
    
    private:
        rfJoinFailedReason m_nReason;
};

#endif // CJOINFAILEDPACKET_H
