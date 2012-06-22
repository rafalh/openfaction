/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CNameChangePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CNameChangePacket.h"
#include "CServer.h"

CNameChangePacket::CNameChangePacket(CServer *pServer, CInputBinaryStream &Stream)
{
    m_pPlayer = pServer->GetPlayersMgr()->Get(Stream.ReadUInt8());
    m_strName = Stream.ReadString();
}

void CNameChangePacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_pPlayer->GetId());
    Stream.WriteString(m_strName);
    
    pPlayer->GetConnection()->AddPacket(RF_NAME_CHANGE, Stream.str(), true);
}
