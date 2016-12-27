#include <tables/CTablesMgr.h>
#include <CVirtualFileSystem.h>
#include <tables/CStringsTable.h>
#include <tables/CAmmoTable.h>
#include <tables/CWeaponsTable.h>
#include <tables/CItemsTable.h>
#include <tables/CEntitiesTable.h>
#include <tables/CClutterTable.h>
#include <tables/CGameTable.h>
#include <tables/CMpCharactersTable.h>
#include <tables/CFoleyTable.h>
#include <util/CException.h>

CTablesMgr::~CTablesMgr()
{
    delete m_pStringsTbl;
    delete m_pAmmoTbl;
    delete m_pWeaponsTbl;
    delete m_pItemsTbl;
    delete m_pEntitiesTbl;
    delete m_pClutterTbl;
    delete m_pGameTbl;
    delete m_pMpCharactersTbl;
    delete m_pFoleyTbl;
}

void CTablesMgr::LoadTables()
{
    CVfsFileStream Stream;

    /* Load strings table */
    m_pStringsTbl = new CStringsTable;
    Stream.Open("strings.tbl");
    assert(Stream.good());
    int iStatus = m_pStringsTbl->Load(Stream);
    if (iStatus < 0)
        THROW_EXCEPTION("Failed to load strings.tbl");

    /* Load ammo table */
    m_pAmmoTbl = new CAmmoTable;
    Stream.Open("ammo.tbl");
    assert(Stream.good());
    iStatus = m_pAmmoTbl->Load(Stream);
    if (iStatus < 0)
        THROW_EXCEPTION("Failed to load ammo.tbl");

    /* Load weapons table */
    m_pWeaponsTbl = new CWeaponsTable(this);
    Stream.Open("weapons.tbl");
    iStatus = m_pWeaponsTbl->Load(Stream);
    if (iStatus < 0)
        THROW_EXCEPTION("Failed to load weapons.tbl");

    /* Load items table */
    m_pItemsTbl = new CItemsTable(this);
    Stream.Open("items.tbl");
    iStatus = m_pItemsTbl->Load(Stream);
    if (iStatus < 0)
        THROW_EXCEPTION("Failed to load items.tbl");

    /* Load entities table */
    m_pEntitiesTbl = new CEntitiesTable;
    Stream.Open("entity.tbl");
    iStatus = m_pEntitiesTbl->Load(Stream);
    if (iStatus < 0)
        THROW_EXCEPTION("Failed to load entity.tbl");

    /* Load clutter table */
    m_pClutterTbl = new CClutterTable;
    Stream.Open("clutter.tbl");
    iStatus = m_pClutterTbl->Load(Stream);
    if (iStatus < 0)
        THROW_EXCEPTION("Failed to load clutter.tbl");
    
    /* Load game table */
    m_pGameTbl = new CGameTable;
    Stream.Open("game.tbl");
    iStatus = m_pGameTbl->Load(Stream);
    if (iStatus < 0)
        THROW_EXCEPTION("Failed to load game.tbl");

    /* Load multiplayer characters table */
    m_pMpCharactersTbl = new CMpCharactersTable(this);
    Stream.Open("pc_multi.tbl");
    iStatus = m_pMpCharactersTbl->Load(Stream);
    if (iStatus < 0)
        THROW_EXCEPTION("Failed to load pc_multi.tbl");

    /* Load foley sounds table */
    m_pFoleyTbl = new CFoleyTable;
    Stream.Open("foley.tbl");
    iStatus = m_pFoleyTbl->Load(Stream);
    if (iStatus < 0)
        THROW_EXCEPTION("Failed to load pc_multi.tbl");
}
