/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CEntitySpawnPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CEntitySpawnPacket.h"
#include "tables/CEntitiesTable.h"
#include "tables/CMpCharactersTable.h"
#include "CWeapon.h"
#include "formats/rfproto.h"

using namespace std;

void CEntitySpawnPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteString(""); //m_pPlayer ? m_pPlayer->GetName() : ""; // Note: Native client does not use it - save bandwidth
    Stream.WriteUInt8((m_pPlayer && m_pPlayer->IsBlueTeam()) ? 1 : 0);
    Stream.WriteUInt8(m_pEntity->GetClass()->nId); // miner1
    Stream.WriteUInt32(m_pEntity->GetHandle());
    Stream.WriteUInt32(0xFFFFFFFF);
    Stream.WriteVector(m_pEntity->GetPos());
    
    Stream.WriteVector(m_pEntity->GetRotMatrix().getColumn(0));
    Stream.WriteVector(m_pEntity->GetRotMatrix().getColumn(1));
    Stream.WriteVector(m_pEntity->GetRotMatrix().getColumn(2));
    
    Stream.WriteUInt8(m_pPlayer ? m_pPlayer->GetId() : 0xFF);
    Stream.WriteUInt32((m_pPlayer && m_pPlayer->GetCharacter()) ? m_pPlayer->GetCharacter()->nId : 5);
    Stream.WriteUInt32(m_pEntity->GetWeapon()->GetClass()->nId);
    Stream.WriteUInt32(0xFFFFFFFF);
    
    pPlayer->GetConnection()->AddPacket(RF_ENTITY_SPAWN, Stream.str(), true);
}
