/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CPlayerConnection.h
*  PURPOSE:     Packets queue and connection mantaining
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CPLAYERCONNECTION_H
#define CPLAYERCONNECTION_H

#include <string>
#include "CSocket.h"
#include "COutputBinaryStream.h"
#include "main.h"

class CPlayer;

class CPlayerConnection
{
    public:
        CPlayerConnection(CPlayer *pPlayer, const CSocketAddress &Addr);
        ~CPlayerConnection();
        void AddSpecialReliablePacket(unsigned nType, std::string strData = "", unsigned nId = 0, unsigned nTicks = g_AppTimer.GetValue());
        void AddPacket(unsigned nType, std::string strData, bool bReliable);
        void FlushPackets();
        void AcceptReliablePacket(unsigned nPacketId);
        
        const CSocketAddress &GetAddr() const
        {
            return m_Addr;
        }
        
    private:
        struct SReliablePacketDesc
        {
            unsigned nTicks;
            std::string strData;
        };
        
        void SendReliablePacket(unsigned nId, SReliablePacketDesc *pDesc);
        
        CPlayer *m_pPlayer;
        const CSocketAddress m_Addr;
        unsigned m_cReliablePackets;
        COutputBinaryStringStream m_PacketsStream;
        bool m_bReliablePackets;
        std::map<unsigned, SReliablePacketDesc> m_WaitingReliablePackets;
        std::map<unsigned, SReliablePacketDesc> m_SentReliablePackets;
};

#endif // CPLAYERCONNECTION_H
