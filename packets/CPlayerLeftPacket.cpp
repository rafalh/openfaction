/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CPlayerLeftPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CPlayerLeftPacket.h"
#include "CServer.h"

CPlayerLeftPacket::CPlayerLeftPacket(CServer *pServer, CInputBinaryStream &Stream)
{
    m_pPlayer = pServer->GetPlayersMgr()->Get(Stream.ReadUInt8());
    m_nReason = (rfLeftReason)Stream.ReadUInt8();
}

void CPlayerLeftPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_pPlayer->GetId());
    Stream.WriteUInt8(m_nReason);
    
    pPlayer->GetConnection()->AddPacket(RF_PLAYER_LEFT, Stream.str(), true);
}
