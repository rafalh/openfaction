/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CGame.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CGAME_H
#define CGAME_H

#ifdef OF_CLIENT
#include <irrlicht.h>
#endif // OF_CLIENT

class CLevel;
class CMeshMgr;
class CMaterialsMgr;
class CSoundManager;
class CConsole;
class CVirtualFileSystem;
class CEventsHandler;

class CStringsTable;
class CAmmoTable;
class CWeaponsTable;
class CEntitiesTable;
class CItemsTable;
class CClutterTable;
class CGameTable;
class CMpCharactersTable;
class CFoleyTable;

namespace irr
{
    class IrrlichtDevice;
    namespace scene
    {
        class ISceneManager;
        class ICameraSceneNode;
    }
    namespace video
    {
        class IVideoDriver;
    }
    namespace gui
    {
        class IGUIEnvironment;
    }
}

class CGame
{
    public:
        struct SConfig
        {
            SConfig():
                bWeaponsStay(false) {}
            
            bool bWeaponsStay;
        };
        
        CGame(CConsole *pConsole, irr::IrrlichtDevice *pIrrDevice = 0);
        ~CGame();
        void LoadTables();
        void InitVfs();
        void LoadMod(const char *pszModName);
        void LoadLevel(const char *pszLevelFilename);
        
        CConsole *GetConsole() const
        {
            return m_pConsole;
        }
        
        CLevel *GetLevel() const
        {
            return m_pLevel;
        }
        
        CMeshMgr *GetMeshMgr() const
        {
            return m_pMeshMgr;
        }
        
        CMaterialsMgr *GetMaterialsMgr() const
        {
            return m_pMaterialsMgr;
        }
        
        CSoundManager *GetSoundMgr() const
        {
            return m_pSoundMgr;
        }
        
        CVirtualFileSystem *GetVfs() const
        {
            return m_pVfs;
        }
        
        CEventsHandler *GetEventsHandler() const
        {
            return m_pEventsHandler;
        }
        
        void SetEventsHandler(CEventsHandler *pEventsHandler)
        {
            m_pEventsHandler = pEventsHandler;
        }
        
        const SConfig &GetConfig() const
        {
            return m_Config;
        }
        
        void SetConfig(SConfig Config)
        {
            m_Config = Config;
        }
        
        // TABLES
        
        CStringsTable *GetStringsTbl() const
        {
            return m_pStringsTbl;
        }
        
        CAmmoTable *GetAmmoTbl() const
        {
            return m_pAmmoTbl;
        }
        
        CWeaponsTable *GetWeaponsTbl() const
        {
            return m_pWeaponsTbl;
        }
        
        CItemsTable *GetItemsTbl() const
        {
            return m_pItemsTbl;
        }
        
        CEntitiesTable *GetEntitiesTbl() const
        {
            return m_pEntitiesTbl;
        }
        
        CClutterTable *GetClutterTbl() const
        {
            return m_pClutterTbl;
        }
        
        CGameTable *GetGameTbl() const
        {
            return m_pGameTbl;
        }
        
        CMpCharactersTable *GetMpCharactersTbl() const
        {
            return m_pMpCharactersTbl;
        }
        
        CFoleyTable *GetFoleyTbl() const
        {
            return m_pFoleyTbl;
        }
        
        // IRRLICHT
        
        irr::scene::ISceneManager *GetSceneMgr() const
        {
#ifdef OF_CLIENT
            if(m_pIrrDevice)
                return m_pIrrDevice->getSceneManager();
#endif // OF_CLIENT
            return 0;
        }
        
        irr::video::IVideoDriver *GetVideoDriver() const
        {
#ifdef OF_CLIENT
            if(m_pIrrDevice)
                return m_pIrrDevice->getVideoDriver();
#endif // OF_CLIENT
            return 0;
        }
        
        irr::gui::IGUIEnvironment *GetGuiEnv() const
        {
#ifdef OF_CLIENT
            if(m_pIrrDevice)
                return m_pIrrDevice->getGUIEnvironment();
#endif // OF_CLIENT
            return 0;
        }
        
        irr::scene::ICameraSceneNode *GetCamera() const
        {
#ifdef OF_CLIENT
            if(m_pIrrDevice)
                return m_pIrrDevice->getSceneManager()->getActiveCamera();
#endif
            return 0;
        }
    
    private:
        CConsole *m_pConsole;
        CLevel *m_pLevel;
        CMeshMgr *m_pMeshMgr;
        CMaterialsMgr *m_pMaterialsMgr;
        CSoundManager *m_pSoundMgr;
        CVirtualFileSystem *m_pVfs;
        CEventsHandler *m_pEventsHandler;
        SConfig m_Config;
        irr::IrrlichtDevice *m_pIrrDevice;
        
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

#endif // CGAME_H
