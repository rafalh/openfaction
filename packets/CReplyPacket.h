/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CReplyPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef _CREPLYPACKET_H
#define _CREPLYPACKET_H

#include "precomp.h"
#include "formats/rfproto.h"

class CReplyPacket
{
    public:
        inline CReplyPacket(unsigned nPacketId, unsigned nTicks):
            m_nPacketId(nPacketId), m_nTicks(nTicks) {}
        
        inline void Send(CPlayer *pPlayer) const
        {
            COutputBinaryStringStream Stream;
            
            Stream.WriteUInt16(m_nPacketId);
            Stream.WriteUInt16(0);
            
            pPlayer->GetConnection()->AddSpecialReliablePacket(RF_REL_REPLY, Stream.str(), 0, m_nTicks);
        }
        
    private:
        unsigned m_nPacketId, m_nTicks;
};

#endif // _CREPLYPACKET_H
