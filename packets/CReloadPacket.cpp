/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CReloadPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CReloadPacket.h"
#include "CEntity.h"
#include "CWeapon.h"

void CReloadPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt32(m_pPlayer->GetEntity()->GetHandle());
    CWeapon *pWeapon = m_pPlayer->GetEntity()->GetWeapon();
    Stream.WriteUInt32(pWeapon->GetClass()->nId);
    Stream.WriteUInt32(m_pPlayer->GetEntity()->GetAmmo());
    Stream.WriteUInt32(pWeapon->GetAmmo());
    
    pPlayer->GetConnection()->AddPacket(RF_RELOAD, Stream.str(), false);
}
