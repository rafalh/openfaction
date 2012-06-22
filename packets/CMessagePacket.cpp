/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CMessagePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CMessagePacket.h"
#include "CServer.h"
#include "CLogger.h"

using namespace std;

void CMessagePacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_pPlayer ? m_pPlayer->GetId() : RF_SERVER);
    Stream.WriteUInt8(m_bTeamMsg ? 1 : 0);
    Stream.WriteString(m_strMessage);
    
    pPlayer->GetConnection()->AddPacket(RF_MESSAGE, Stream.str(), false);
}

CMessagePacket::CMessagePacket(CServer *pServer, CInputBinaryStream &Stream)
{
    unsigned nPlayerId = Stream.ReadUInt8();
    m_pPlayer = pServer->GetPlayersMgr()->Get(nPlayerId);
    if(!m_pPlayer)
        CLogger::GetInst().PrintError("Invalid player: 0x%02x", nPlayerId);
    
    m_bTeamMsg = Stream.ReadUInt8() ? true : false;
    m_strMessage = Stream.ReadString();
}
