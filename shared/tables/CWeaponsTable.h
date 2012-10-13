/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CWeaponsTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CWEAPONSTABLE_H
#define CWEAPONSTABLE_H

#include <vector>
#include <map>
#include "CString.h"
#include "CAmmoTable.h"

class CGame;

enum EWeaponFlags
{
    OFW_ALT_FIRE         = 1 << 0,
    OFW_CONT_FIRE        = 1 << 1,
    OFW_ALT_CONT_FIRE    = 1 << 2,
    OFW_FLICKERS         = 1 << 3,
    OFW_THRUSTER         = 1 << 4,
    OFW_MELEE            = 1 << 5,
    OFW_REMOTE_CHARGE    = 1 << 6,
    OFW_PLAYER_WEP       = 1 << 7,
    OFW_ALT_ZOOM         = 1 << 8,
    OFW_UNDERWATER       = 1 << 9,
    OFW_FROM_EYE         = 1 << 10,
    OFW_GRAVITY          = 1 << 11,
    OFW_FIXED_MUZ_FLASH  = 1 << 12,
    OFW_ALT_LOCK         = 1 << 13,
    OFW_SILENT           = 1 << 14,
    OFW_DRILL            = 1 << 15,
    OFW_DETONATOR        = 1 << 16,
    OFW_ALT_CUSTOM_MODE  = 1 << 17,
    OFW_PS2_FP_FULL_CLIP = 1 << 18,
    OFW_NO_WORLD_COLLIDE = 1 << 19,
};

enum EWeaponFlags2
{
    OFW_PIERCES_ALL        = 1 << 0,
    OFW_RANDOM_BMP_ORIENT  = 1 << 1,
    OFW_CYCLE_ALPHA        = 1 << 2,
    OFW_NO_FIRE_THROUGH    = 1 << 3,
    OFW_NO_WORLD_COLLIDE2  = 1 << 4,
    OFW_UNDEVIATING        = 1 << 5,
    OFW_FLAME              = 1 << 6,
    OFW_HAS_SCANNER        = 1 << 7,
    OFW_DAMAGE_SELF        = 1 << 8,
    OFW_HAS_PILOT_FLAME    = 1 << 9,
    OFW_MULTI_MESH_COLLIDE = 1 << 10,
};

enum EWeaponType
{
    WT_CLOSE_COMBAT,
    WT_SEMI_AUTO,
    WT_HEAVY,
    WT_EXPLOSIVE,
};

enum EDamageType
{
    WDT_BASH,
    WDT_BULLET,
    WDT_ARMOR_PIERCING_BULLET,
    WDT_EXPLOSIVE,
    WDT_FIRE,
    WDT_ENERGY,
    WDT_ELECTRICAL,
    WDT_ACID,
    WDT_SCALDING,
};

struct SWeaponClass
{
    struct SAction
    {
        CString strAnim;
        CString strSound;
    };
    
    SWeaponClass(unsigned nId);
    
    unsigned nId;
    CString strName;
    CString strDisplayName;
    unsigned nFlags, nFlags2;
    unsigned iCyclePos, iPrefPos;
    EWeaponType WeaponType;
    CString strWeaponIcon;
    EDamageType DmgType;
    std::map<CString, SAction> Actions;
    CString strHudReticleFilename;
    const SAmmoType *pAmmoType; // can be NULL
    unsigned nMaxAmmo, nClipSize;
    float fMass, fVelocity, fColRadius;
    unsigned nClipReloadTime, nClipDrainTime;
    unsigned nFireWait, nAltFireWait;
    unsigned nLifeTime;
    unsigned cProjectiles;
    float fDamage, fDamageMulti;
    float fAltDamage, fAltDamageMulti;
    unsigned nBurstCount;
    bool bBurstAltFire;
    CString strLaunch, strAltLaunch, strLaunchFail;
    float fDamageRadius, fDamageRadiusMulti;
    unsigned nThrustLifeTime;
};

class CWeaponsTable
{
    public:
        CWeaponsTable(CGame *pGame):
            m_pGame(pGame) {}
        int Load(std::istream &Stream);
        const SWeaponClass *Get(const CString &strName) const;
        
        inline const SWeaponClass *Get(unsigned nId) const
        {
            if(nId >= m_Weapons.size())
                return NULL;
            
            return &m_Weapons[nId];
        }
    
    private:
        CGame *m_pGame;
        std::vector<SWeaponClass> m_Weapons;
};

#endif // CWEAPONSTABLE_H
