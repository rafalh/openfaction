/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CServerInfoPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CServerInfoPacket.h"
#include "COutputBinaryStream.h"
#include "CServer.h"
#include "CPlayersManager.h"
#include "CLevel.h"
#include "CServerConfig.h"

void CServerInfoPacket::Send(CServer *pServer, const CSocketAddress &Addr) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(RF_VER_12);
    Stream.WriteString(pServer->GetConf()->GetName());
    Stream.WriteUInt8(pServer->GetConf()->GetGameType());
    Stream.WriteUInt8(pServer->GetPlayersMgr()->GetCount());
    Stream.WriteUInt8(pServer->GetConf()->GetMaxPlayers());
    Stream.WriteString(pServer->GetLevel()->GetName());
    Stream.WriteString(pServer->GetConf()->GetMod());
    unsigned nFlags = RF_SIF_NOT_LAN | RF_SIF_DEDICATED;
    if(pServer->GetConf()->GetPassword()[0])
        nFlags |= RF_SIF_PASSWORD;
    Stream.WriteUInt8(nFlags);
    
    COutputBinaryStringStream Stream2;
    
    Stream2.WriteUInt8(RF_GAME);
    Stream2.WriteUInt8(RF_SERVER_INFO);
    Stream2.WriteUInt16((uint16_t)Stream.str().size());
    
    Stream2.WriteBinary(Stream.str());
    
#if MASK_AS_PF
    /* Note: PF doesn't use RF protocol properly for its packets */
    Stream2.WriteBinary("\xDE\xAD\xBE\xEF\xDF\x30\xE8\x04\x00\x8C\x03\x00\x01", 13);
#endif
    
    pServer->GetSocket().SendTo(Stream2.str(), Addr);
}
