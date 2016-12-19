/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CGame.h
*  PURPOSE:     Main class containing references to all useful objects
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CGAME_H
#define CGAME_H

#include "tables/CTablesMgr.h"
#ifdef OF_CLIENT
# include <irrlicht.h>
#endif // OF_CLIENT

class CLevel;
class CMeshMgr;
class CMaterialsMgr;
class CAnimMgr;
class CSoundManager;
class CConsole;
class CVirtualFileSystem;
class CEventsHandler;
class CCamera;
class CTablesMgr;

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
        
        CAnimMgr *GetAnimMgr() const
        {
            return m_pAnimMgr;
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

        CTablesMgr *GetTables() const
        {
            return m_pTablesMgr;
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
        
        CCamera *GetCamera() const
        {
            return m_pCamera;
        }
        
        void SetCamera(CCamera *pCamera)
        {
            m_pCamera = pCamera;
        }
    
    private:
        CConsole *m_pConsole;
        CLevel *m_pLevel;
        CMeshMgr *m_pMeshMgr;
        CMaterialsMgr *m_pMaterialsMgr;
        CAnimMgr *m_pAnimMgr;
        CSoundManager *m_pSoundMgr;
        CVirtualFileSystem *m_pVfs;
        CEventsHandler *m_pEventsHandler;
        SConfig m_Config;
        CCamera *m_pCamera;
        irr::IrrlichtDevice *m_pIrrDevice;
        CTablesMgr *m_pTablesMgr;
};

#endif // CGAME_H
