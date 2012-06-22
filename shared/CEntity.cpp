/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CEntity.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CGame.h"
#include "CEventsHandler.h"
#include "CEntity.h"
#include "CWeapon.h"
#include "CHandleManager.h"
#include "CTrigger.h"
#include "CItem.h"
#include "CColWorld.h"
#include "CLevel.h"
#include "CMesh.h"
#include "CMeshMgr.h"
#include "CEntitiesTable.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

using namespace std;
#ifdef OF_CLIENT
using namespace irr;
#endif // OF_CLIENT

CEntity::CEntity(CLevel *pLevel, const SEntityClass *pClass, unsigned nUid):
    CKillableObject(OFET_ENTITY, pLevel, nUid),
    m_pClass(pClass),
    m_fArmor(0.0f), m_bLifeArmorChanged(true),
    m_CamRotMatrix(btMatrix3x3::getIdentity()), m_nAnimation(0), m_bPosRotAnimChanged(true),
    m_State(CEntityState::ES_STAND), m_Action(CEntityAction::EA_IDLE_1),
    m_pWeapon(NULL), m_bWeaponChanged(true),
    m_nInvulnerability(0), m_nDamageAmplifier(0), m_bArmorStateChanged(true),
    m_pPlayer(NULL),
    m_pColShape(NULL), m_pCharacter(NULL)
{
    m_iColGroup = COL_ENTITY;
    m_iColMask = COL_LEVEL;
    
    m_fLife = m_pClass->fLife;
    m_Handle = CHandleManager::GetInst().AllocHandle();
    
    m_pMesh = m_pLevel->GetGame()->GetMeshMgr()->Load(m_pClass->strMeshFilename.c_str());
    
    btConvexShape *pColShape = m_pMesh->GetMultiColSphere();
    if(!pColShape)
        pColShape = m_pColShape = new btSphereShape(m_pClass->fCollisionRadius);
#if 1 // dynamic character
    btVector3 vInertia(0.0f, 0.0f, 0.0f); // character doesn't have inertia
    pColShape->calculateLocalInertia(m_pClass->fMass, vInertia);
    btRigidBody::btRigidBodyConstructionInfo ConstructionInfo(m_pClass->fMass, &m_MotionState, pColShape, vInertia);
    m_pColObj = new btRigidBody(ConstructionInfo);
    btRigidBody::upcast(m_pColObj)->setAngularFactor(0.0f);
#else // kinematic character
    m_pColObj = new btPairCachingGhostObject();
    m_pColObj->setCollisionShape(pColShape);
    
    const float fStepHeight = 0.35f;
	m_pCharacter = new btKinematicCharacterController((btPairCachingGhostObject*)m_pColObj, pColShape, fStepHeight);
	//m_pCharacter->setFallSpeed(0.1f);
	
	m_pLevel->GetColWorld()->GetColWorld()->addAction(m_pCharacter);
#endif
    m_pColObj->setUserPointer(this);
    
    m_pColObj->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    
    m_pColObj->setFriction(0.1f);
    m_pColObj->setRestitution(0.0f);
    
    CAmmoTable *pAmmoTbl = m_pLevel->GetGame()->GetAmmoTbl();
    m_Ammo = new unsigned[pAmmoTbl->GetCount()];
    for(unsigned i = 0; i < pAmmoTbl->GetCount(); ++i)
        m_Ammo[i] = 0;
    
    const SWeaponClass *pWeaponCls = m_pLevel->GetGame()->GetWeaponsTbl()->Get(RF_PISTOL);
    m_pWeapon = AddWeapon(pWeaponCls, 0);
    
#ifdef OF_CLIENT
    scene::IMesh *pIrrMesh = m_pMesh->GetSubMesh(0)->GetIrrMesh();
    assert(pIrrMesh);
    m_pSceneNode = m_pLevel->GetGame()->GetSceneMgr()->addMeshSceneNode(pIrrMesh);
#endif // OF_CLIENT
    
    AddToWorld();
}

CEntity::CEntity(CLevel *pLevel, CInputBinaryStream &Stream):
    CKillableObject(OFET_ENTITY, pLevel),
    m_fArmor(0.0f), m_bLifeArmorChanged(true),
    m_nAnimation(0), m_bPosRotAnimChanged(true),
    m_State(CEntityState::ES_STAND), m_Action(CEntityAction::EA_IDLE_1),
    m_bWeaponChanged(true),
    m_nInvulnerability(0), m_nDamageAmplifier(0), m_bArmorStateChanged(true),
    m_pPlayer(NULL),
    m_pColShape(NULL), m_pCharacter(NULL)
{
    m_iColGroup = COL_ENTITY;
    m_iColMask = COL_LEVEL;
    
    m_Handle = CHandleManager::GetInst().AllocHandle();
    
    m_nUid = Stream.ReadUInt32();
    
    string strClassName = Stream.ReadString2();
    m_pClass = m_pLevel->GetGame()->GetEntitiesTbl()->Get(strClassName.c_str());
    assert(m_pClass);
    
    m_pMesh = m_pLevel->GetGame()->GetMeshMgr()->Load(m_pClass->strMeshFilename.c_str());
    assert(m_pMesh->GetSubMeshCount() > 0);
    
    btConvexShape *pColShape = m_pMesh->GetMultiColSphere();
    if(!pColShape)
        pColShape = m_pColShape = new btSphereShape(m_pClass->fCollisionRadius);
    
#if 1 // dynamic character
    btVector3 vInertia(0.0f, 0.0f, 0.0f); // character doesn't have inertia
    pColShape->calculateLocalInertia(m_pClass->fMass, vInertia);
    btRigidBody::btRigidBodyConstructionInfo ConstructionInfo(m_pClass->fMass, &m_MotionState, pColShape, vInertia);
    m_pColObj = new btRigidBody(ConstructionInfo);
    btRigidBody::upcast(m_pColObj)->setAngularFactor(0.0f);
#else // kinematic character
    m_pColObj = new btPairCachingGhostObject();
    m_pColObj->setCollisionShape(m_pColShape);
    
    const float fStepHeight = 0.35f;
	m_pCharacter = new btKinematicCharacterController((btPairCachingGhostObject*)m_pColObj, pColShape, fStepHeight);
	m_pCharacter->setFallSpeed(0.01f);
	
	m_pLevel->GetColWorld()->GetColWorld()->addAction(m_pCharacter);
#endif

    m_pColObj->setUserPointer(this);
    m_pColObj->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    
    m_pColObj->setFriction(1.0f);
    m_pColObj->setRestitution(0.0f);
    
    SetPos(Stream.ReadVector());
    
    btMatrix3x3 matRot;
    matRot[2] = Stream.ReadVector();
    matRot[0] = Stream.ReadVector();
    matRot[1] = Stream.ReadVector();
    m_CamRotMatrix = matRot.transpose();
    
    Stream.ReadString2(); // script name
    Stream.ignore(13); // unknown, cooperation, friendliness, team_id
    Stream.ReadString2(); // waypoint list
    Stream.ReadString2(); // waypoint method
    Stream.ignore(17); // unknown2, boarded, ready_to_fire_state, only_attack_player, weapon_is_holstered,
                       // deaf, sweep_min_angle, sweep_max_angle, ignore_terrain_when_firing, unknown3, start_crouched
    m_fLife = Stream.ReadFloat();
    m_fArmor = Stream.ReadFloat();
    Stream.ignore(4); // fov
    
    CAmmoTable *pAmmoTbl = m_pLevel->GetGame()->GetAmmoTbl();
    m_Ammo = new unsigned[pAmmoTbl->GetCount()];
    for(unsigned i = 0; i < pAmmoTbl->GetCount(); ++i)
        m_Ammo[i] = 0;
    
    string strPrimaryWeapon = Stream.ReadString2();
    unsigned cAmmo;
    const SWeaponClass *pWeaponCls = m_pLevel->GetGame()->GetWeaponsTbl()->Get(strPrimaryWeapon.c_str());
    if(!pWeaponCls)
    {
        pWeaponCls = m_pLevel->GetGame()->GetWeaponsTbl()->Get(RF_PISTOL);
        cAmmo = 0;
    } else
        cAmmo = (pWeaponCls->nId == RF_PISTOL ? 64 : pWeaponCls->nClipSize);
    
    m_pWeapon = AddWeapon(pWeaponCls, cAmmo);
    
    Stream.ReadString2(); // secondary weapon
    Stream.ReadString2(); // item drop
    Stream.ReadString2(); // state anim
    Stream.ReadString2(); // corpse pose
    Stream.ReadString2(); // skin
    Stream.ReadString2(); // death anim
    Stream.ignore(34); // ai_mode, ai_attack_style, unknown4, turret_uid, alert_camera_uid, alarm_event_uid, run,
                       // start_hidden, wear_helmet, end_game_if_killed, cower_from_weapon, question_unarmed_player,
                       // dont_hum, no_shadow, always_simulate, perfect_aim, permanent_corpse, never_fly, never_leave,
                       // no_persona_messages, fade_corpse_immediately, never_collide_with_player
    bool bUseCustomAttackRange = Stream.ReadUInt8() ? true : false;
    if(bUseCustomAttackRange)
        Stream.ignore(4); // custom attack range
    Stream.ReadString2(); // left hand holding
    Stream.ReadString2(); // right hand holding
    
#ifdef OF_CLIENT
    scene::IMesh *pIrrMesh = m_pMesh->GetSubMesh(0)->GetIrrMesh();
    assert(pIrrMesh);
    m_pSceneNode = m_pLevel->GetGame()->GetSceneMgr()->addMeshSceneNode(pIrrMesh);
#endif // OF_CLIENT
    
    AddToWorld();
}

CEntity::~CEntity()
{
    RemoveWeapons();
    
    if(m_pCharacter)
    {
        m_pLevel->GetColWorld()->GetColWorld()->removeAction(m_pCharacter);
        delete m_pCharacter;
        m_pCharacter = NULL;
    }
    
    if(m_pMesh)
        m_pMesh->Release();
    m_pMesh = NULL;
    
    delete[] m_Ammo;
    m_Ammo = NULL;
    
    delete m_pColShape;
    m_pColShape = NULL;
}

void CEntity::Kill(SDamageInfo &DmgInfo)
{
    if(!IsAlive())
        return; // already dead
    
    m_fLife = 0.0f;
    m_fArmor = 0.0f;
    
    m_pLevel->GetGame()->GetEventsHandler()->OnEntityKill(this, DmgInfo);
    
    /* Remove collision object from world */
    RemoveFromWorld();
    if(m_pCharacter)
    {
        m_pLevel->GetColWorld()->GetColWorld()->removeAction(m_pCharacter);
        delete m_pCharacter;
        m_pCharacter = NULL;
    }
}

void CEntity::SetLife(float fLife)
{
    if(GetLife() != fLife)
    {
        CKillableObject::SetLife(fLife);
        m_bLifeArmorChanged = true;
    }
}

void CEntity::SetArmor(float fArmor)
{
    if(m_fArmor != fArmor)
    {
        m_fArmor = fArmor;
        m_bLifeArmorChanged = true;
    }
}

void CEntity::Damage(float fDmg, SDamageInfo &DmgInfo)
{
    if(m_fLife > 0.0f && fDmg > 0.0f)
    {
        /* Note: armor is 2x stronger than life */
        if(fDmg > m_fArmor * 2.0f)
        {
            fDmg -= m_fArmor * 2.0f;
            m_fArmor = 0.0f;
            
            CKillableObject::Damage(fDmg, DmgInfo);
        }
        else
            m_fArmor -= fDmg / 2.0f;
        
        m_bLifeArmorChanged = true;
    }
}

void CEntity::Update(unsigned nDeltaTime)
{
    if(m_fLife != 0.0f)
    {
        if(m_pWeapon)
            m_pWeapon->Update(nDeltaTime);
        
        if(nDeltaTime >= m_nInvulnerability)
            SetInvulnerability(0);
        else
            m_nInvulnerability -= nDeltaTime;
        
        if(nDeltaTime > m_nDamageAmplifier)
            SetDamageAmplifier(0);
        else
            m_nDamageAmplifier -= nDeltaTime;
        
        map<CObject*, SColInfo> Objects = m_pLevel->GetColWorld()->SweepTest(m_OldTransform, m_pColObj->getWorldTransform(), COL_ITEM|COL_TRIGGER, m_pClass->fCollisionRadius);
        m_OldTransform = m_pColObj->getWorldTransform();
        
        map<CObject*, SColInfo>::iterator it;
        for(it = Objects.begin(); it != Objects.end(); ++it)
        {
            CItem *pItem = dynamic_cast<CItem*>(it->first);
            CTrigger *pTrigger = dynamic_cast<CTrigger*>(it->first);
            assert(pItem || pTrigger);
            
            if(pItem)
            {
                bool bCanApply = true;
                bCanApply = (m_pLevel->GetGame()->GetConfig().bWeaponsStay || !pItem->GetClass()->bWeapon || pItem->GetBit() == 0xFFFF ||
                            !GetWeapon(pItem->GetClass()->pWeaponClass));
                if(bCanApply)
                    pItem->OnPickup(this);
            }
            else if(pTrigger)
            {
                if(pTrigger->IsEnabled() && !pTrigger->IsUseKeyRequired() && !pTrigger->IsAuto())
                {
                    bool bActivated = pTrigger->Activate(this);
                    if(bActivated)
                        m_pLevel->GetGame()->GetEventsHandler()->OnTiggerActivate(pTrigger, this);
                }
            }
        }
    }
    
    CObject::Update(nDeltaTime);
}

unsigned CEntity::GetAmmo(const SAmmoType *pAmmoType)
{
    if(!pAmmoType)
        pAmmoType = m_pWeapon->GetClass()->pAmmoType;
    
    return pAmmoType ? m_Ammo[pAmmoType->nId] : 0;
}

bool CEntity::SwitchWeapon(const SWeaponClass *pClass)
{
    if(pClass == m_pWeapon->GetClass())
        return true;
    
    std::map<unsigned, CWeapon*>::iterator it = m_Weapons.find(pClass->nId);
    if(it == m_Weapons.end())
        return false;
    
    m_pWeapon = it->second;
    m_bWeaponChanged = true;
    
    return true;
}

void CEntity::RemoveWeapons()
{
    map<unsigned, CWeapon*>::iterator it;
    for(it = m_Weapons.begin(); it != m_Weapons.end(); ++it)
        delete it->second;
    m_Weapons.clear();
    
    for(unsigned i = 0; i < m_pLevel->GetGame()->GetAmmoTbl()->GetCount(); ++i)
        m_Ammo[i] = 0;
    
    m_pWeapon = NULL;
}

bool CEntity::AddAmmo(const SWeaponClass *pWeaponCls, unsigned nAmmo)
{
    const SAmmoType *pAmmoType = pWeaponCls->pAmmoType;
    if(!pAmmoType || m_Ammo[pAmmoType->nId] >= pWeaponCls->nMaxAmmo)
        return false;
    
    m_Ammo[pAmmoType->nId] = min(m_Ammo[pAmmoType->nId] + nAmmo, pWeaponCls->nMaxAmmo);
    return true;
}

CWeapon *CEntity::AddWeapon(const SWeaponClass *pClass, unsigned cAmmo)
{
    CWeapon *pWeapon;
    map<unsigned, CWeapon*>::iterator it = m_Weapons.find(pClass->nId);
    if(it == m_Weapons.end())
    {
        pWeapon = new CWeapon(pClass, this, min(cAmmo, pClass->nClipSize));
        m_Weapons.insert(pair<unsigned, CWeapon*>(pClass->nId, pWeapon));
        cAmmo -= min(cAmmo, pClass->nClipSize);
    } else
        pWeapon = it->second;
    
    if(cAmmo)
        AddAmmo(pClass, cAmmo);
    
    if(!m_pWeapon)
        m_pWeapon = pWeapon;
    
    return pWeapon;
}
        
bool CEntity::ReloadWeapon(const SWeaponClass *pClass, bool bWait)
{
    CWeapon *pWeapon = pClass ? GetWeapon(pClass) : m_pWeapon;
    if(!pWeapon)
        return false;
    
    return pWeapon->Reload(bWait);
}

bool CEntity::Shoot(bool bAltFire, bool bWait)
{
    btVector3 vDir = m_CamRotMatrix.getColumn(2); // forward vector
    btVector3 vPos = GetHeadPos();
    
    return Shoot(vPos, vDir, bAltFire, bWait);
}

bool CEntity::Shoot(const btVector3 &vPos, const btVector3 &vDir, bool bAltFire, bool bWait)
{
    if(!m_pWeapon)
        return false;
    
    return m_pWeapon->Shoot(bAltFire, vPos, vDir, bWait);
}

void CEntity::DbgDraw() const
{
    if(m_pMesh)
        m_pMesh->DbgDraw(this);
}
