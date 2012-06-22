/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CItem.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CGame.h"
#include "CEventsHandler.h"
#include "utils.h"
#include "CItem.h"
#include "CEntity.h"
#include "CHandleManager.h"
#include "CLevel.h"
#include "CMesh.h"
#include "CMeshMgr.h"
#ifdef OF_CLIENT
# include "CSoundManager.h"
#endif

using namespace std;
#ifdef OF_CLIENT
using namespace irr;
#endif

btSphereShape CItem::m_ColShape(1.0f);

CItem::CItem(CLevel *pLevel, const SItemClass *pClass, unsigned nBit, unsigned nUid):
    CObject(OFET_ITEM, pLevel, nUid),
    m_pClass(pClass), m_nBit(nBit), m_nCount(0),
    m_nRespawnTime(0), m_nDisableTime(0),
    m_fRotMulti(0.0f)
{
    m_iColGroup = COL_ITEM;
    m_iColMask = COL_NONE;
    
    m_pColObj = new btCollisionObject();
    m_pColObj->setUserPointer(this);
    m_pColObj->setCollisionShape(&m_ColShape);
    
    m_Handle = CHandleManager::GetInst().AllocHandle();
    
    m_pMesh = NULL;
    AddToWorld();
}

CItem::CItem(CLevel *pLevel, CInputBinaryStream &Stream, unsigned nBit):
    CObject(OFET_ITEM, pLevel),
    m_nBit(nBit), m_nDisableTime(0), m_fRotMulti(0.0f)
{
    m_iColGroup = COL_ITEM;
    m_iColMask = COL_NONE;
    
    m_pColObj = new btCollisionObject();
    m_pColObj->setUserPointer(this);
    m_pColObj->setCollisionShape(&m_ColShape);
    
    m_Handle = CHandleManager::GetInst().AllocHandle();
    
    m_nUid = Stream.ReadUInt32();
    
    string strClassName = Stream.ReadString2();
    m_pClass = m_pLevel->GetGame()->GetItemsTbl()->Get(strClassName.c_str());
    assert(m_pClass);
    
    SetPos(Stream.ReadVector());
    
    btMatrix3x3 matRot;
    matRot[2] = Stream.ReadVector();
    matRot[0] = Stream.ReadVector();
    matRot[1] = Stream.ReadVector();
    SetRotMatrix(matRot.transpose());
    
    Stream.ReadString2(); // script name
    Stream.ignore(1); // zero
    
    /* Note: native server uses count values from items.tbl.
       Values in map are only for single player mode */
    //Item.SetCount(Stream.ReadUInt32());
    Stream.ignore(4); // count
    m_nCount = m_pClass->nCount;
    
    m_nRespawnTime = Stream.ReadUInt32();
    Stream.ignore(4); // team id
    
    m_pMesh = m_pLevel->GetGame()->GetMeshMgr()->Load(m_pClass->strMeshFilename.c_str());
    if(m_pMesh)
    {
        assert(m_pMesh->GetSubMeshCount() > 0);

#ifdef OF_CLIENT
        scene::IMesh *pIrrMesh = m_pMesh->GetSubMesh(0)->GetIrrMesh();
        assert(pIrrMesh);
        m_pSceneNode = m_pLevel->GetGame()->GetSceneMgr()->addMeshSceneNode(pIrrMesh);
#endif // OF_CLIENT
    }
    
    AddToWorld();
}

CItem::~CItem()
{
    if(m_pMesh)
        m_pMesh->Release();
    m_pMesh = NULL;
}

void CItem::SetEnabled(bool bEnabled)
{
    if(!bEnabled && !m_nDisableTime)
    {
        if(m_nRespawnTime)
            m_nDisableTime = m_nRespawnTime * 1000;
        else
            m_pLevel->QueueDelete(this);
        RemoveFromWorld();
    }
    else if(bEnabled && m_nDisableTime)
    {
        m_nDisableTime = 0;
        AddToWorld();
    }
}

bool CItem::CanApply(CEntity *pEntity)
{
    const SWeaponClass *pWeaponCls = m_pClass->pWeaponClass;
    if(pWeaponCls)
    {
        CWeapon *pWeapon = pEntity->GetWeapon(pWeaponCls);
        
        if(m_pLevel->GetGame()->GetConfig().bWeaponsStay && m_pClass->bWeapon && pWeapon)
            return false;
        
        if(pWeapon && pEntity->GetAmmo(pWeaponCls->pAmmoType) == pWeaponCls->nMaxAmmo)
            return false;
    }
    else if(m_pClass->Type == IT_MEDICAL_KIT)
    {
        if(pEntity->GetLife() >= 100.0f)
            return false;
    }
    else if(m_pClass->Type == IT_MULTI_SUPER_HEALTH)
    {
        if(pEntity->GetLife() >= 200.0f)
            return false;
    }
    else if(m_pClass->Type == IT_SUIT_REPAIR)
    {
        if(pEntity->GetArmor() >= 100.0f)
            return false;
    }
    else if(m_pClass->Type == IT_MINER_ENVIROSUIT)
    {
        if(pEntity->GetArmor() >= 100.0f)
            return false;
    }
    else if(m_pClass->Type == IT_MULTI_SUPER_ARMOR)
    {
        if(pEntity->GetArmor() >= 200.0f)
            return false;
    }
    
    return true;
}

bool CItem::OnPickup(CEntity *pEntity)
{
    if(m_pClass->Flags & IF_NO_PICKUP)
        return false;
    
    if(!CanApply(pEntity))
        return false;
    
    if(m_pLevel->GetGame()->GetEventsHandler()->OnItemApply(this, pEntity))
        return true;
    
#ifdef OF_CLIENT
    CSoundManager *pSoundMgr = m_pLevel->GetGame()->GetSoundMgr();
    if(pSoundMgr)
    {
        if(m_pClass->pWeaponClass)
            pSoundMgr->PlaySound("weapon_pickup.wav");
        else
            pSoundMgr->PlaySound("powerup_pickup.wav");
    }
#endif // OF_CLIENT
    
    Apply(pEntity);
    
    if(m_nRespawnTime)
        SetEnabled(false);
    else
        m_pLevel->QueueDelete(this);
    
    return true;
}

void CItem::Apply(CEntity *pEntity)
{
    const SWeaponClass *pWeaponCls = m_pClass->pWeaponClass;
    if(pWeaponCls)
    {
        CWeapon *pWeapon = pEntity->GetWeapon(pWeaponCls);
        
        if(m_pClass->bWeapon && !pWeapon)
            pWeapon = pEntity->AddWeapon(pWeaponCls, m_nCount);
        else
            pEntity->AddAmmo(pWeaponCls, m_nCount);
    }
    else if(m_pClass->Type == IT_MEDICAL_KIT)
        pEntity->SetLife(min(pEntity->GetLife() + (float)m_nCount, 100.0f));
    else if(m_pClass->Type == IT_MULTI_SUPER_HEALTH)
        pEntity->SetLife(200.0f);
    else if(m_pClass->Type == IT_SUIT_REPAIR)
        pEntity->SetArmor(min(pEntity->GetArmor() + (float)m_nCount, 100.0f));
    else if(m_pClass->Type == IT_MINER_ENVIROSUIT)
        pEntity->SetArmor(100.0f);
    else if(m_pClass->Type == IT_MULTI_SUPER_ARMOR)
        pEntity->SetArmor(200.0f);
    else if(m_pClass->Type == IT_MULTI_DMG_AMP)
        pEntity->SetDamageAmplifier(m_nCount);
    else if(m_pClass->Type == IT_MULTI_INVULNERABILITY)
        pEntity->SetInvulnerability(m_nCount);
}

void CItem::Update(unsigned nDeltaTime)
{
    if(m_nDisableTime > nDeltaTime)
        m_nDisableTime -= nDeltaTime;
    else if(m_nDisableTime)
    {
        m_nDisableTime = 0;
        AddToWorld();
    }
    
#ifdef OF_CLIENT
    m_fRotMulti += nDeltaTime / 300.0f;
    if(m_pSceneNode && (m_pClass->Flags & IF_SPINS_IN_MULTI))
    {
        /*btMatrix3x3 matRot;
        matRot.setEulerYPR(0, m_fRotMulti, 0);
        SetRotMatrix(matRot);*/
        m_pSceneNode->setRotation(core::vector3df(0.0f, m_fRotMulti / M_PI * 180.0f, 0.0f));
    }
#endif // OF_CLIENT
    
    CObject::Update(nDeltaTime);
}
