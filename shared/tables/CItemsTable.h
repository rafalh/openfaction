/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CItemsTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CITEMSTABLE_H
#define CITEMSTABLE_H

#include "CWeaponsTable.h"

enum EItemFlags
{
    IF_NO_PICKUP      = 1 << 0,
    IF_SPINS_IN_MULTI = 1 << 1,
};

enum EItemType
{
    IT_WEAPON, IT_AMMO,
    IT_MEDICAL_KIT, IT_MULTI_SUPER_HEALTH, IT_SUIT_REPAIR, IT_MINER_ENVIROSUIT,
    IT_MULTI_SUPER_ARMOR, IT_MULTI_DMG_AMP, IT_MULTI_INVULNERABILITY,
    IT_UNKNOWN
};

struct SItemClass
{
    SItemClass(unsigned nId);
    
    unsigned nId;
    CString strName;
    EItemType Type;
    CString strMeshFilename;
    const SWeaponClass *pWeaponClass;
    bool bWeapon;
    unsigned nCount;
    unsigned Flags;
};

class CItemsTable
{
    public:
        CItemsTable(CGame *pGame):
            m_pGame(pGame) {}
        int Load(std::istream &Stream);
        const SItemClass *Get(const CString &strClassName);
        const SItemClass *FindWeapon(const SWeaponClass *pClass);
    
    private:
        CGame *m_pGame;
        std::vector<SItemClass> m_Items;
        
        static EItemType GetTypeFromClsName(const CString &strItemClsName);
};

#endif // CITEMSTABLE_H
