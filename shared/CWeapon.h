/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CWeapon.h
*  PURPOSE:     Entity weapon
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CWEAPON_H
#define CWEAPON_H

#include "CTimer.h"
#include "tables/CWeaponsTable.h"
#include <btBulletCollisionCommon.h>
#include <algorithm>

class CEntity;
struct SWeaponClass;

class CWeapon
{
    public:
        enum EState { WS_IDLE, WS_FIRE, WS_ALT_FIRE };
        
        CWeapon(const SWeaponClass *pClass, CEntity *pEntity, unsigned nAmmo = 0);
        bool Shoot(bool bAltFire, const btVector3 &vPos, const btVector3 &vDir, bool bWait = true);
        void Update(unsigned nDeltaTime);
        bool Reload(bool bWait = true);
        void DoAction(const CString &strName);
        
        inline const SWeaponClass *GetClass() const
        {
            return m_pClass;
        }
        
        inline unsigned GetAmmo() const
        {
            return m_cClipAmmo;
        }
        
        inline void AddAmmo(unsigned nAmmo)
        {
            m_cClipAmmo = std::min(m_cClipAmmo + nAmmo, m_pClass->nClipSize);
        }
        
        inline void SetState(EState State)
        {
            m_State = State;
        }
        
        inline EState GetState() const
        {
            return m_State;
        }
    
    private:
        const SWeaponClass *m_pClass;
        bool m_bShotgun;
        CEntity *m_pEntity;
        unsigned m_cClipAmmo;
        EState m_State;
        unsigned m_nWait;
        bool m_bQueuedShoot, m_bQueuedAltShoot;
};

#endif // CWEAPON_H
