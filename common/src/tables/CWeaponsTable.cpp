/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CWeaponsTable.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "util/utils.h"
#include "tables/CWeaponsTable.h"
#include "tables/CTblReader.h"
#include "tables/CTablesMgr.h"

using namespace std;

static const char *g_WeaponFlags[] = {
    "alt_fire", "continuous_fire", "alt_continuous_fire", "flickers",
    "thruster", "melee", "remote_charge", "player_wep",
    "alt_zoom", "underwater", "from_eye", "gravity",
    "fixed_muz_flash", "alt_lock", "silent", "drill",
    "detonator", "alt_custom_mode", "ps2_fp_full_clip", "no_world_collide",
    "semi_automatic", "autoaim", "infrared", "torpedo"};
static const char *g_WeaponFlags2[] = {
    "pierces_all", "random_bmp_orient", "cycle_alpha", "no_fire_through",
    "no_world_collide", "undeviating", "flame", "has_scanner",
    "damage_self", "has_pilot_flame", "multi_mesh_collide"};
static const char *g_WeaponTypes[] = {
    "close combat", "semi-auto", "heavy", "explosive"};
static const char *g_DmgTypes[] = {
    "bash", "bullet", "armor piercing bullet", "explosive",
    "fire", "energy", "electrical", "acid",
    "scalding"};

SWeaponClass::SWeaponClass(unsigned _nId)
{
    nId = _nId;
    nFlags = 0;
    nFlags2 = 0;
    iCyclePos = 0;
    iPrefPos = 0;
    WeaponType = WT_SEMI_AUTO;
    DmgType = WDT_BULLET;
    pAmmoType = NULL;
    nMaxAmmo = 0;
    nClipSize = 0;
    fMass = 0.1f;
    fVelocity = 1.0f;
    fColRadius = 0.01f;
    nClipReloadTime = 0;
    nClipDrainTime = 0;
    nLifeTime = 0;
    cProjectiles = 1;
    nFireWait = 0;
    nAltFireWait = 0;
    fDamage = 0.0f;
    fDamageMulti = 0.0f;
    fAltDamage = 0.0f;
    fAltDamageMulti = 0.0f;
    nBurstCount = 1;
    bBurstAltFire = false;
    fDamageRadius = 0.0f;
    fDamageRadiusMulti = 0.0f;
    nThrustLifeTime = 0;
}

int CWeaponsTable::Load(istream &Stream)
{
    CTblReader Reader(Stream, "Primary Weapons");
    if(!Reader.IsSectionFound())
        return -1;
    
    float fTemp;
    
    while(Reader.LoadNextElement() == 0)
    {
        const char *pName = Reader.GetName();
        if(!StrCmpI(pName, "$Name"))
        {
            SWeaponClass Weapon(m_Weapons.size());
            Reader.GetString(Weapon.strName);
            
            m_Weapons.push_back(Weapon);
        }
        else if(m_Weapons.size() > 0)
        {
            if(!StrCmpI(pName, "$Display Name"))
            {
                Reader.GetString(m_Weapons.back().strDisplayName, m_tablesMgr->strings());
                if(m_Weapons.back().strDisplayName.empty())
                    m_Weapons.back().strDisplayName = "(empty)";
            }
            else if(!StrCmpI(pName, "$Flags"))
                Reader.GetFlags(m_Weapons.back().nFlags, g_WeaponFlags, COUNTOF(g_WeaponFlags));
            else if(!StrCmpI(pName, "$Flags2"))
                Reader.GetFlags(m_Weapons.back().nFlags2, g_WeaponFlags2, COUNTOF(g_WeaponFlags2));
            else if(!StrCmpI(pName, "$Cycle Position"))
                Reader.GetUInt(m_Weapons.back().iCyclePos);
            else if(!StrCmpI(pName, "$Pref Position"))
                Reader.GetUInt(m_Weapons.back().iPrefPos);
            else if(!StrCmpI(pName, "$Weapon Type"))
                Reader.GetEnum((int&)m_Weapons.back().WeaponType, g_WeaponTypes, COUNTOF(g_WeaponTypes));
            else if(!StrCmpI(pName, "$Weapon Icon"))
                Reader.GetString(m_Weapons.back().strWeaponIcon);
            else if(!StrCmpI(pName, "$Damage Type"))
                Reader.GetEnum((int&)m_Weapons.back().DmgType, g_DmgTypes, COUNTOF(g_DmgTypes));
            else if(!StrCmpI(pName, "+Action"))
            {
                CString strName;
                SWeaponClass::SAction Action;
                Reader.GetString(strName);
                Reader.GetString(Action.strAnim);
                Reader.GetString(Action.strSound);
                m_Weapons.back().Actions[strName] = Action;
            }
            else if(!StrCmpI(pName, "$HUD Reticle Filename"))
            {
                Reader.GetString(m_Weapons.back().strHudReticleFilename);
            }
            else if(!StrCmpI(pName, "$Ammo Type"))
            {
                CString strAmmoType;
                Reader.GetString(strAmmoType);
                if(!strAmmoType.empty())
                    m_Weapons.back().pAmmoType = m_tablesMgr->ammo()->Get(strAmmoType);
            }
            else if(!StrCmpI(pName, "$Max Ammo"))
            {
                Reader.GetUInt(m_Weapons.back().nMaxAmmo);
                Reader.GetUInt(m_Weapons.back().nMaxAmmo); // multiplayer
            }
            else if(!StrCmpI(pName, "$Clip Size"))
            {
                Reader.GetUInt(m_Weapons.back().nClipSize);
                Reader.GetUInt(m_Weapons.back().nClipSize); // multiplayer
            }
            else if(!StrCmpI(pName, "$Mass"))
                Reader.GetFloat(m_Weapons.back().fMass);
            else if(!StrCmpI(pName, "$Velocity"))
                Reader.GetFloat(m_Weapons.back().fVelocity);
            else if(!StrCmpI(pName, "$Collision Radius"))
                Reader.GetFloat(m_Weapons.back().fColRadius);
            else if(!StrCmpI(pName, "$Clip Reload Time"))
            {
                Reader.GetFloat(fTemp);
                m_Weapons.back().nClipReloadTime = (unsigned)(fTemp * 1000);
            }
            else if(!StrCmpI(pName, "$Fire Wait"))
            {
                Reader.GetFloat(fTemp);
                m_Weapons.back().nFireWait = (unsigned)(fTemp * 1000);
            }
            else if(!StrCmpI(pName, "$Alt Fire Wait"))
            {
                Reader.GetFloat(fTemp);
                m_Weapons.back().nAltFireWait = (unsigned)(fTemp * 1000);
            }
            else if(!StrCmpI(pName, "$Clip Drain Time"))
            {
                Reader.GetFloat(fTemp);
                m_Weapons.back().nClipDrainTime =(unsigned)(fTemp * 1000);
            }
            else if(!StrCmpI(pName, "$Lifetime"))
            {
                Reader.GetFloat(fTemp);
                m_Weapons.back().nLifeTime =(unsigned)(fTemp * 1000);
            }
            else if(!StrCmpI(pName, "$Num Projectiles"))
                Reader.GetUInt(m_Weapons.back().cProjectiles);
            else if(!StrCmpI(pName, "$Damage"))
            {
                Reader.GetFloat(m_Weapons.back().fDamage);
                m_Weapons.back().fDamageMulti = m_Weapons.back().fDamage;
                m_Weapons.back().fAltDamage = m_Weapons.back().fDamage;
                m_Weapons.back().fAltDamageMulti = m_Weapons.back().fDamage;
            }
            else if(!StrCmpI(pName, "$Damage Multi"))
                Reader.GetFloat(m_Weapons.back().fDamageMulti);
            else if(!StrCmpI(pName, "$Alt Damage"))
            {
                Reader.GetFloat(m_Weapons.back().fAltDamage);
                m_Weapons.back().fAltDamageMulti = m_Weapons.back().fAltDamage;
            }
            else if(!StrCmpI(pName, "$Alt Damage Multi"))
                Reader.GetFloat(m_Weapons.back().fAltDamageMulti);
            else if(!StrCmpI(pName, "+Burst Count"))
                Reader.GetUInt(m_Weapons.back().nBurstCount);
            else if(!StrCmpI(pName, "+Burst Alt Fire"))
                Reader.GetBool(m_Weapons.back().bBurstAltFire);
            else if(!StrCmpI(pName, "$Launch"))
                Reader.GetString(m_Weapons.back().strLaunch);
            else if(!StrCmpI(pName, "$Alt Launch"))
                Reader.GetString(m_Weapons.back().strAltLaunch);
            else if(!StrCmpI(pName, "$Launch Fail"))
                Reader.GetString(m_Weapons.back().strLaunchFail);
            else if(!StrCmpI(pName, "$Damage Radius"))
            {
                Reader.GetFloat(m_Weapons.back().fDamageRadius);
                m_Weapons.back().fDamageRadiusMulti = m_Weapons.back().fDamageRadius;
            }
            else if(!StrCmpI(pName, "$Damage Radius Multi"))
                Reader.GetFloat(m_Weapons.back().fDamageRadiusMulti);
            else if(!StrCmpI(pName, "$Thrust Lifetime"))
            {
                Reader.GetFloat(fTemp);
                m_Weapons.back().nThrustLifeTime = (unsigned)(fTemp * 1000);
            }
        }
    }
    
    return 0;
}

const SWeaponClass *CWeaponsTable::Get(const CString &strName) const
{
    for(unsigned i = 0; i < m_Weapons.size(); ++i)
        if(!m_Weapons[i].strName.comparei(strName))
            return &m_Weapons[i];
    
    return NULL;
}
