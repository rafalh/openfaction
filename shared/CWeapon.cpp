/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CWeapon.cpp
*  PURPOSE:     Entity weapon
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CGame.h"
#include "rfproto.h"
#include "utils.h"
#include "CWeapon.h"
#include "CConsole.h"
#include "CProjectile.h"
#include "CLevel.h"
#include "CEntity.h"
#ifdef OF_CLIENT
# include "CSoundManager.h"
#endif // OF_CLIENT

using namespace std;

CWeapon::CWeapon(const SWeaponClass *pClass, CEntity *pEntity, unsigned nAmmo):
    m_pClass(pClass), m_bShotgun(false), m_pEntity(pEntity),
    m_cClipAmmo(nAmmo),
    m_State(WS_IDLE),
    m_nWait(0), m_bQueuedShoot(false), m_bQueuedAltShoot(false)
{
    if(m_cClipAmmo > m_pClass->nClipSize)
        m_cClipAmmo = m_pClass->nClipSize;
    
    if(!m_pClass->strName.comparei("Shotgun"))
        m_bShotgun = true;
}

bool CWeapon::Shoot(bool bAltFire, const btVector3 &vPos, const btVector3 &vDir, bool bWait)
{
	/* Does weapon support alt fire? */
    if(bAltFire && !(m_pClass->nFlags & OFW_ALT_FIRE))
        return false;
    
    if(m_pClass->nFlags & OFW_DETONATOR)
    {
        // TODO
        m_pEntity->GetLevel()->GetGame()->GetConsole()->DbgPrint("Detonate remote charge...\n");
        const SWeaponClass *pRemoteChargeCls = m_pEntity->GetLevel()->GetGame()->GetWeaponsTbl()->Get(RF_REMOTE_CHARGE);
        if(pRemoteChargeCls)
            m_pEntity->SwitchWeapon(pRemoteChargeCls);
        return true;
    }
    
    // Retreive amount of ammo
    unsigned cAmmo = (m_pClass->nClipSize > 0) ? m_cClipAmmo : m_pEntity->GetAmmo(m_pClass->pAmmoType);
    
	/* If it is melee don't check ammo */
	bool bMelee = !bAltFire && (m_pClass->nFlags & OFW_MELEE) != 0;
	if(!bMelee && !cAmmo)
	{
#ifdef OF_CLIENT
	    CSoundManager *pSoundMgr = m_pEntity->GetLevel()->GetGame()->GetSoundMgr();
        if(pSoundMgr)
            pSoundMgr->PlayFoleySound(m_pClass->strLaunchFail);
#endif // OF_CLIENT
        
	    DoAction("fire_fail");
	    
	    return false;
	}
        
    
	/* Queue one shoot if it's not time to shoot */
    if(bWait && m_nWait > 0)
    {
        bool &bQueued = bAltFire ? m_bQueuedShoot : m_bQueuedAltShoot;
        
        if(bQueued)
            return false;
        
        bQueued = true;
        return true;
    }
    
	/* Set wait value so entity can't shoot too fast */
    m_nWait = bAltFire ? m_pClass->nAltFireWait : m_pClass->nFireWait;
    unsigned cProjectiles = m_pClass->cProjectiles;
    
    /* Calculate projectiles count and remove ammo */
    unsigned cBullets = 0;
    if(!bMelee)
    {
        if(bAltFire && (m_pClass->nFlags2 & OFW_FLAME))
        {
            if(cAmmo == m_pClass->nClipSize)
            {
                cAmmo = 0;
                cBullets = m_pClass->nClipSize;
            } else
                cProjectiles = 0;
        }
        else if(m_pClass->nClipDrainTime > 0)
        {
            cAmmo -= min(m_pClass->nClipSize / (m_pClass->nClipDrainTime / m_nWait), cAmmo);
        }
        else if(m_pClass->nBurstCount != 1 && m_pClass->bBurstAltFire == bAltFire)
        {
            cProjectiles *= min(m_pClass->nBurstCount, cAmmo);
            cAmmo -= min(m_pClass->nBurstCount, cAmmo);
        }
        else if(!bAltFire && m_bShotgun) // it seems shotgun is hardcoded in RF
        {
            cProjectiles *= min(2u, cAmmo);
            cAmmo -= min(2u, cAmmo);
        }
        else
            --cAmmo;
        
        if(m_pClass->nClipSize > 0)
            m_cClipAmmo = cAmmo;
        else
            m_pEntity->SetAmmo(m_pClass->pAmmoType, cAmmo);
    }
    
    for(unsigned i = 0; i < cProjectiles; ++i)
    {
        CProjectile *pProjectile = new CProjectile(m_pEntity->GetLevel(), m_pClass, bAltFire, m_pEntity);
        pProjectile->SetPos(vPos);
        pProjectile->SetDir(vDir);
    }
    
    if(m_pClass->nFlags & OFW_REMOTE_CHARGE)
    {
        const SWeaponClass *pDetonatorCls = m_pEntity->GetLevel()->GetGame()->GetWeaponsTbl()->Get(RF_REMOTE_CHARGE_DETONATOR);
        if(pDetonatorCls)
        {
            m_pEntity->AddWeapon(pDetonatorCls);
            m_pEntity->SwitchWeapon(pDetonatorCls);
            m_pEntity->GetLevel()->GetGame()->GetConsole()->DbgPrint("Remote charge thrown...\n");
        }
    }
    
#ifdef OF_CLIENT
    CSoundManager *pSoundMgr = m_pEntity->GetLevel()->GetGame()->GetSoundMgr();
    if(pSoundMgr)
        pSoundMgr->PlayFoleySound(bAltFire ? m_pClass->strAltLaunch : m_pClass->strLaunch);
#endif // OF_CLIENT
    
    DoAction("fire");
    
    return true;
}

void CWeapon::Update(unsigned nDeltaTime)
{
    btVector3 vPos = m_pEntity->GetHeadPos();
    btVector3 vDir = m_pEntity->GetRotMatrix().getColumn(2); // forward vector
    
    while(nDeltaTime)
    {
        if(m_nWait > nDeltaTime)
        {
            m_nWait -= nDeltaTime;
            break;
        }

        nDeltaTime -= m_nWait;
        m_nWait = 0;
        
        if(m_State == WS_FIRE || m_bQueuedShoot)
		{
            if(!Shoot(false, vPos, vDir, m_pEntity))
				break; // Nothing to do
			
            m_bQueuedShoot = false;
        }
        else if(m_State == WS_ALT_FIRE || m_bQueuedAltShoot)
        {
            if(!Shoot(true, vPos, vDir, m_pEntity))
				break; // Nothing to do

            m_bQueuedAltShoot = false;
        }
        else
            break;
    }
}

bool CWeapon::Reload(bool bWait)
{
    const SAmmoType *pAmmoType = m_pClass->pAmmoType;
    unsigned cTotalAmmo = pAmmoType ? m_pEntity->GetAmmo(pAmmoType) : 0;
    if(cTotalAmmo == 0)
        return false;
    
    if(m_nWait > 0 && bWait)
        return false;
    m_nWait = m_pClass->nClipReloadTime;
    
    unsigned cClipAmmoAdd = std::min(cTotalAmmo, m_pClass->nClipSize - m_cClipAmmo);
    
    // Add ammo to clip
    m_cClipAmmo += cClipAmmoAdd;
    
    // Remove ammo from entity
    cTotalAmmo -= cClipAmmoAdd;
    m_pEntity->SetAmmo(pAmmoType, cTotalAmmo);
    
    m_pEntity->SetAction(CEntityAction::EA_RELOAD);
    DoAction("reload");
    
    return true;
}

void CWeapon::DoAction(const CString &strName)
{
    map<CString, SWeaponClass::SAction>::const_iterator it = m_pClass->Actions.find(strName);
    if(it == m_pClass->Actions.end())
        return;
    
#ifdef OF_CLIENT
    CSoundManager *pSoundMgr = m_pEntity->GetLevel()->GetGame()->GetSoundMgr();
    if(pSoundMgr)
        pSoundMgr->PlayFoleySound(it->second.strSound);
#endif // OF_CLIENT
}
