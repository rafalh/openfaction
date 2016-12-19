/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  PURPOSE:     Main class containing references to all useful objects
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#pragma once

class CStringsTable;
class CAmmoTable;
class CWeaponsTable;
class CEntitiesTable;
class CItemsTable;
class CClutterTable;
class CGameTable;
class CMpCharactersTable;
class CFoleyTable;

class CTablesMgr
{
public:
    ~CTablesMgr();
    void LoadTables();

    CStringsTable *strings() const
    {
        return m_pStringsTbl;
    }

    CAmmoTable *ammo() const
    {
        return m_pAmmoTbl;
    }

    CWeaponsTable *weapons() const
    {
        return m_pWeaponsTbl;
    }

    CItemsTable *items() const
    {
        return m_pItemsTbl;
    }

    CEntitiesTable *entities() const
    {
        return m_pEntitiesTbl;
    }

    CClutterTable *clutter() const
    {
        return m_pClutterTbl;
    }

    CGameTable *game() const
    {
        return m_pGameTbl;
    }

    CMpCharactersTable *characters() const
    {
        return m_pMpCharactersTbl;
    }

    CFoleyTable *foley() const
    {
        return m_pFoleyTbl;
    }

private:
    CStringsTable *m_pStringsTbl;
    CAmmoTable *m_pAmmoTbl;
    CWeaponsTable *m_pWeaponsTbl;
    CItemsTable *m_pItemsTbl;
    CEntitiesTable *m_pEntitiesTbl;
    CClutterTable *m_pClutterTbl;
    CGameTable *m_pGameTbl;
    CMpCharactersTable *m_pMpCharactersTbl;
    CFoleyTable *m_pFoleyTbl;
};
