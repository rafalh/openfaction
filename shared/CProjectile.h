/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CProjectile.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CPROJECTILE_H
#define CPROJECTILE_H

#include "CWeaponsTable.h"
#include "CObject.h"

class CEntity;

class CProjectile: public CObject
{
    public:
        CProjectile(CLevel *pLevel, const SWeaponClass *pWeaponCls, bool bAltFire, CEntity *pEntity = NULL);
        void Update(unsigned nDeltaTime);
        void Explode(const btVector3 &vPos);
        
        inline CEntity *GetEntity() const
        {
            return m_pEntity;
        }
        
        inline CEntity *SetEntity(CEntity *pEntity)
        {
            m_pEntity = pEntity;
        }
        
        inline void SetDir(const btVector3 &vDir)
        {
            m_vDir = vDir;
            ((btRigidBody*)m_pColObj)->setLinearVelocity(vDir.normalized() * m_pWeaponCls->fVelocity);
        }
        
        inline const btVector3 &GetDir() const
        {
            return m_vDir;
        }
        
        inline unsigned GetLifeTime() const
        {
            return std::max((int)(m_pWeaponCls->nLifeTime - m_nTime), 0);
        }
        
        void SetPos(const btVector3 &vPos)
        {
            CObject::SetPos(vPos);
            m_OldTransform = m_pColObj->getWorldTransform();
        }
        
    private:
        const SWeaponClass *m_pWeaponCls;
        bool m_bAltFire;
        btVector3 m_vDir;
        btTransform m_OldTransform;
        unsigned m_nTime;
        class CEntity *m_pEntity;
        
        btSphereShape m_ColShape;
        btDefaultMotionState m_MotionState;
};

#endif // CPROJECTILE_H
