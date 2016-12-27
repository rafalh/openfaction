/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CGlassKillPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CGlassKillPacket.h"
#include "CStaticGeometry.h"
#include "formats/rfproto.h"

void CGlassKillPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt32(m_pRoom->GetUid());
    Stream.WriteUInt8(m_bExplosion ? 1 : 0);
    Stream.WriteVector(m_vPos);
    Stream.WriteVector(btVector3());
    
    pPlayer->GetConnection()->AddPacket(RF_GLASS_KILL, Stream.str(), true);
}
