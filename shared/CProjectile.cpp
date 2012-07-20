/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CProjectile.cpp
*  PURPOSE:     Projectile element
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CProjectile.h"
#include "CLevel.h"
#include "CClutter.h"
#include "CEntity.h"
#include "CColWorld.h"
#include "CElementsMgr.h"
#include "utils.h"
#include "CConsole.h"
#include "CStaticGeometry.h"
#include "CGame.h"
#ifdef OF_CLIENT
using namespace irr;
#endif // OF_CLIENT

using namespace std;

// Note: We can't use Bullet methods of collision detection + CCD because it doesn't work for triggers.

CProjectile::CProjectile(CLevel *pLevel, const SWeaponClass *pWeaponCls, bool bAltFire, CEntity *pEntity):
    CObject(OFET_PROJECTILE, pLevel, OFE_INVALID_UID),
    m_ColShape(pWeaponCls->fColRadius),
    m_pWeaponCls(pWeaponCls), m_bAltFire(bAltFire),
    m_pEntity(pEntity)
{
    assert(pWeaponCls);
    
    m_nTime = 0;
    
    //btVector3 vLocalInertia;
    //m_ColShape.calculateLocalInertia(pWeaponCls->fMass, vLocalInertia);
    m_pColObj = new btRigidBody(pWeaponCls->fMass, &m_MotionState, &m_ColShape);
    m_pColObj->setUserPointer(this);
    ((btRigidBody*)m_pColObj)->setSleepingThresholds(0.1f, 0.1f);
    
    if(!(pWeaponCls->nFlags & OFW_GRAVITY))
        ((btRigidBody*)m_pColObj)->setFlags(BT_DISABLE_WORLD_GRAVITY);
    
    m_pColObj->setCollisionFlags(m_pColObj->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    //m_pColObj->setCcdMotionThreshold(0.01f);
    //m_pColObj->setCcdSweptSphereRadius(pWeaponCls->fColRadius);
    
    AddToWorld();
    
#ifdef OF_CLIENT
    m_pSceneNode = m_pLevel->GetGame()->GetSceneMgr()->addSphereSceneNode(pWeaponCls->fColRadius);
#endif
}

void CProjectile::Update(unsigned nDeltaTime)
{
    m_nTime += nDeltaTime;
    
    if(m_pWeaponCls->nThrustLifeTime > 0 && m_nTime >= m_pWeaponCls->nThrustLifeTime && (((btRigidBody*)m_pColObj)->getFlags() & BT_DISABLE_WORLD_GRAVITY))
    {
        ((btRigidBody*)m_pColObj)->setFlags(((btRigidBody*)m_pColObj)->getFlags() & ~BT_DISABLE_WORLD_GRAVITY);
        btVector3 vecGravity = m_pLevel->GetColWorld()->GetColWorld()->getGravity();
        ((btRigidBody*)m_pColObj)->setGravity(vecGravity);
    }
    
    map<CObject*, SColInfo> Objects = m_pLevel->GetColWorld()->SweepTest(m_OldTransform, m_pColObj->getWorldTransform(), COL_ENTITY|COL_LEVEL|COL_CLUTTER, m_pWeaponCls->fColRadius, m_pEntity);
    m_OldTransform = m_pColObj->getWorldTransform();
    
    SDamageInfo DmgInfo;
    DmgInfo.pResponsibleObj = m_pEntity;
    DmgInfo.Flags = 0;
    
    bool bDestroy = false;
    map<CObject*, SColInfo>::iterator it;
    for(it = Objects.begin(); it != Objects.end(); ++it)
    {
        CKillableObject *pObj = dynamic_cast<CKillableObject*>(it->first);
        if(!pObj)
            continue; // not killable
        
        CRoom *pRoom = dynamic_cast<CRoom*>(pObj);
        bool bIsGlass = (pRoom && pRoom->IsGlass());
        bDestroy = bDestroy || !bIsGlass;
        
        DmgInfo.vPos = it->second.vPos;
        float fDmg;
        if(bIsGlass)
            fDmg = m_bAltFire ? m_pWeaponCls->fAltDamage : m_pWeaponCls->fDamage;
        else
        {
            bDestroy = true;
            fDmg = m_bAltFire ? m_pWeaponCls->fAltDamageMulti : m_pWeaponCls->fDamageMulti;
        }
        
        m_pLevel->GetGame()->GetConsole()->DbgPrint("Projectile hit %u (type %u life %.1f), dmg %.1f dist %.1f\n", pObj->GetUid(), pObj->GetType(), pObj->GetLife(), fDmg, (GetPos() - pObj->GetPos()).length());
        if(bIsGlass || m_pWeaponCls->DmgType != WDT_EXPLOSIVE)
            pObj->Damage(fDmg, DmgInfo);
    }
    
    if(m_nTime >= m_pWeaponCls->nLifeTime)
        bDestroy = true;
    
    if(bDestroy)
    {
        if(m_pWeaponCls->DmgType == WDT_EXPLOSIVE)
            Explode(GetPos());
        
        m_pLevel->QueueDelete(this);
    }
    
    CObject::Update(nDeltaTime);
}

void CProjectile::Explode(const btVector3 &vPos)
{
    m_pLevel->GetGame()->GetConsole()->DbgPrint("Explosion %.1f\n", m_pWeaponCls->fDamageRadius);
    
    SDamageInfo DmgInfo;
    DmgInfo.Flags = DMG_EXPLOSION;
    DmgInfo.pResponsibleObj = m_pEntity;
    
    EElementType AffectedTypes[] = {OFET_LEVEL, OFET_ENTITY, OFET_CLUTTER};
    for(unsigned i = 0; i < COUNTOF(AffectedTypes); ++i)
    {
        const set<CElement*> Elements = m_pLevel->GetElementsMgr()->GetByType(AffectedTypes[i]);
        set<CElement*>::iterator it;
        for(it = Elements.begin(); it != Elements.end(); ++it)
        {
            CKillableObject *pObj = dynamic_cast<CKillableObject*>(*it);
            if(!pObj)
                continue; // not killable
            
            float fDist = (pObj->GetPos() - vPos).length();
            
            if(pObj->GetLife() > 0.0f && fDist < m_pWeaponCls->fDamageRadius)
            {
                float fDmg = m_pWeaponCls->fDamageMulti;
                fDmg *= (m_pWeaponCls->fDamageRadius - fDist) / m_pWeaponCls->fDamageRadius;
                
                pObj->Damage(fDmg, DmgInfo);
                m_pLevel->GetGame()->GetConsole()->DbgPrint("Explosion hit %u\n", pObj->GetUid());
            }
        }
    }
    // TODO: find objects which intersects with sphere
}
