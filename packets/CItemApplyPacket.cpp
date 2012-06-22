/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CItemApplyPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CItemApplyPacket.h"
#include "CEntity.h"
#include "CWeapon.h"

void CItemApplyPacket::Send(CPlayer *pPlayer) const
{
    assert(m_pEntity->IsAlive());
    
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt32(m_pItem->GetHandle());
    Stream.WriteUInt32(m_pEntity->GetHandle());
    
    CWeapon *pWeapon = NULL;
    const SWeaponClass *pWeaponClass = m_pItem->GetClass()->pWeaponClass;
    if(pWeaponClass)
        pWeapon = m_pEntity->GetWeapon(pWeaponClass);
    
    if(pWeapon)
    {
        Stream.WriteUInt32(pWeapon->GetClass()->nId);
        Stream.WriteUInt32(pWeapon->GetAmmo());
        Stream.WriteUInt32(m_pEntity->GetAmmo(pWeapon->GetClass()->pAmmoType));
    }
    else
    {
        Stream.WriteUInt32(0xFFFFFFFF);
        Stream.WriteUInt32(0xFFFFFFFF);
        Stream.WriteUInt32(0xFFFFFFFF);
    }
    
    pPlayer->GetConnection()->AddPacket(RF_ITEM_APPLY, Stream.str(), true);
}
