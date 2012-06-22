/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CPlayerConnection.cpp
*  PURPOSE:     Packets queue and connection mantaining
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CPlayerConnection.h"
#include "rfproto.h"
#include "CPlayer.h"
#include "CServer.h"

using namespace std;

CPlayerConnection::CPlayerConnection(CPlayer *pPlayer, const CSocketAddress &Addr):
    m_pPlayer(pPlayer), m_Addr(Addr), m_cReliablePackets(0) {}

CPlayerConnection::~CPlayerConnection()
{
    FlushPackets();
}

void CPlayerConnection::AddPacket(unsigned nType, std::string strData, bool bReliable)
{
    /* Flush packets if packet type changes or packet size reach limit */
    if(m_bReliablePackets != bReliable ||
       m_PacketsStream.str().size() + (m_bReliablePackets ? 11 : 0) + 3 + strData.size() > MAX_PACKET_SIZE-100)
    {
        FlushPackets();
    }
    
    m_bReliablePackets = bReliable;
    
    if(!bReliable && m_PacketsStream.str().empty())
        m_PacketsStream.WriteUInt8(RF_GAME);
    
    m_PacketsStream.WriteUInt8(nType);
    m_PacketsStream.WriteUInt16(strData.size());
    m_PacketsStream.WriteBinary(strData.data(), strData.size());

#if !PACKETS_BUFFERING
    FlushPackets();
#endif
}

void CPlayerConnection::FlushPackets()
{
    /* Resend reliable packets which was not accepted */
    for(map<unsigned, SReliablePacketDesc>::iterator it = m_SentReliablePackets.begin(); it != m_SentReliablePackets.end(); ++it)
        if(g_AppTimer.GetValue() - it->second.nTicks > 5000)
        {
            it->second.nTicks = g_AppTimer.GetValue();
            SendReliablePacket(it->first, &(it->second));
        }
    
    if(!m_PacketsStream.str().empty())
    {
        /* If packets are reliable add header and save them in buffer */
        if(m_bReliablePackets)
        {
            bool bSend = (m_SentReliablePackets.size() < 5) ? true : false;
            
            /* Add to sent or waiting buffer */
            SReliablePacketDesc *pDesc = bSend
                                       ? &m_SentReliablePackets[m_cReliablePackets]
                                       : &m_WaitingReliablePackets[m_cReliablePackets];
            
            pDesc->nTicks = g_AppTimer.GetValue();
            pDesc->strData = m_PacketsStream.str();
            
            /* Clear packets buffer */
            m_PacketsStream.str("");
            
            /* Send packets */
            if(bSend)
                SendReliablePacket(m_cReliablePackets, pDesc);
            
            ++m_cReliablePackets;
        }
        else
        {
            /* Send unreliable packet */
            m_pPlayer->GetServer()->GetSocket().SendTo(m_PacketsStream.str(), m_Addr);
            
            /* Clear packets buffer */
            m_PacketsStream.str("");
        }
    }
}

void CPlayerConnection::AddSpecialReliablePacket(unsigned nType, std::string strData, unsigned nId, unsigned nTicks)
{
    FlushPackets();
    
    m_bReliablePackets = false; // prevent FlushPackets from adding header
    m_PacketsStream.WriteUInt8(RF_RELIABLE);
    m_PacketsStream.WriteUInt8(nType);
    m_PacketsStream.WriteUInt8(0);
    m_PacketsStream.WriteUInt16(nId);
    m_PacketsStream.WriteUInt16(strData.size());
    m_PacketsStream.WriteUInt32(nTicks);
    m_PacketsStream.WriteBinary(strData.data(), strData.size());
    
    FlushPackets();
}

void CPlayerConnection::SendReliablePacket(unsigned nId, SReliablePacketDesc *pDesc)
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(RF_RELIABLE);
    Stream.WriteUInt8(RF_REL_PACKETS);
    Stream.WriteUInt8(0);
    Stream.WriteUInt16(nId);
    Stream.WriteUInt16(pDesc->strData.size());
    Stream.WriteUInt32(pDesc->nTicks);
    Stream.WriteBinary(pDesc->strData.data(), pDesc->strData.size());
    
    m_pPlayer->GetServer()->GetSocket().SendTo(Stream.str(), m_Addr);
}

void CPlayerConnection::AcceptReliablePacket(unsigned nPacketId)
{
    m_SentReliablePackets.erase(nPacketId);
    if(!m_WaitingReliablePackets.empty())
    {
        map<unsigned, SReliablePacketDesc>::iterator it = m_WaitingReliablePackets.begin();
        
        SendReliablePacket(it->first, &(it->second));
        
        m_SentReliablePackets[it->first] = it->second;
        m_WaitingReliablePackets.erase(it);
    }
}
