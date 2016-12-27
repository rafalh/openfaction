/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CClientApp.h
*  PURPOSE:     Game client main class
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCLIENTAPP_H
#define CCLIENTAPP_H

#include <Irrlicht.h>
#include "irr/CGUITTFont.h"
#include "camera/CCamera.h"

class CGame;
class CLevel;
class CEntity;
class CEntityController;
class CClientConsole;
class CHud;
class CWeaponSelection;

class CClientApp: public irr::IEventReceiver
{
    public:
        CClientApp();
        ~CClientApp();
        void Init();
        bool OnEvent(const irr::SEvent &Event);
        void LoadLevel(const char *pszName);
        void Run();
        void SetCameraType(bool bFreeCamera);
        
        void SwitchCamera()
        {
            SetCameraType(!m_bFreeCamera);
        }
        
        CEntity *GetLocalEntity()
        {
            return m_pLocalEntity;
        }
        
        CEntityController *GetLocalEntityController() const
        {
            return m_pEntityController;
        }
        
        irr::scene::ICameraSceneNode *GetCamera() const
        {
            return m_pCamera->GetSceneNode();
        }
        
        CGame *GetGame() const
        {
            return m_pGame;
        }
        
        CClientConsole *GetConsole() const
        {
            return m_pConsole;
        }
    
    private:
        bool m_bQuit;
        
        CGame *m_pGame;
        irr::IrrlichtDevice *m_pDevice;
        irr::scene::ISceneManager *m_pSceneMgr;
        irr::video::IVideoDriver *m_pDriver;
        
        irr::gui::IGUIFont *m_pFont;
        irr::gui::CGUITTFace *m_pFontFace;
        
        //irr::scene::ICameraSceneNode *m_pCamera;
        bool m_bFreeCamera;
        CCamera *m_pCamera;
        
        CClientConsole *m_pConsole;
        
        CEntity *m_pLocalEntity;
        CEntityController *m_pEntityController;
        CHud *m_pHud;
        CWeaponSelection *m_pWeaponSel;
};

#endif // CCLIENTAPP_H
