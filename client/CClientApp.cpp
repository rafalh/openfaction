/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CClientApp.cpp
*  PURPOSE:     Game client main class
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CClientApp.h"
#include "CVirtualFileSystem.h"
#include "CLevel.h"
#include "CColWorld.h"
#include "CLevelProperties.h"
#include "CProjectile.h"
#include "CEntity.h"
#include "camera/CFreeCamera.h"
#include "camera/CEntityCamera.h"
#include "CEntityControler.h"
#include "utils.h"
#include "CException.h"
#include "CConsole.h"
#include "CGame.h"
#include "irr/CGUITTFont.h"
#include "CClientConsole.h"
#include "CLogger.h"
#include "CHud.h"
#include "CWeaponSelection.h"

#include "CWeaponsTable.h"
#include "CEntitiesTable.h"
#include "CGameTable.h"

#include <irrKlang.h>

using namespace std;
using namespace irr;

CClientApp::CClientApp():
    m_bQuit(false),
    m_pGame(NULL),
    m_pDevice(NULL), m_pSceneMgr(NULL), m_pDriver(NULL),
    m_pFont(NULL), m_pFontFace(NULL),
    m_bFreeCamera(false),
    m_pCamera(NULL), m_pConsole(NULL),
    m_pLocalEntity(NULL),
    m_pEntityController(NULL), m_pHud(NULL), m_pWeaponSel(NULL) {}

CClientApp::~CClientApp()
{
    delete m_pHud;
    m_pHud = NULL;
    
    delete m_pWeaponSel;
    m_pWeaponSel = NULL;
    
    delete m_pGame;
    m_pGame = NULL;
    
    if(m_pFont)
        m_pFont->drop();
    m_pFont = NULL;
    
    if(m_pFontFace)
        m_pFontFace->drop();
    m_pFontFace = NULL;
    
    delete m_pConsole;
    m_pConsole = NULL;
    
    //m_pCamera->remove();
    delete m_pCamera;
    m_pCamera = NULL;
    
    if(m_pDevice)
        m_pDevice->drop();
    m_pDevice = NULL;
}

void CClientApp::Init()
{
    m_pDevice = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(800, 600), 32, false, false, false, this);
    //m_pDevice = createDevice(video::EDT_DIRECT3D9, core::dimension2d<u32>(800, 600), 32, false, false, false, 0);
    if(!m_pDevice)
        THROW_EXCEPTION("Failed to create Irrlicht device");
    
    m_pDevice->setWindowCaption(L"OF Client");
    m_pDriver = m_pDevice->getVideoDriver();
    m_pSceneMgr = m_pDevice->getSceneManager();
    
    // Hide cursor
    m_pDevice->getCursorControl()->setVisible(false);
    
    // Load font
    m_pFontFace = new gui::CGUITTFace;
    if(!m_pFontFace->load("arial.ttf"))
    {
        CLogger::GetInst().PrintError("Failed to load font. Using built in font...\n");
        delete m_pFontFace;
        m_pFontFace = NULL;
    }
    
    if(m_pFontFace)
    {
        m_pFont = new gui::CGUITTFont(m_pDevice->getGUIEnvironment());
        static_cast<gui::CGUITTFont*>(m_pFont)->attach(m_pFontFace, 12);
    }
    else
    {
        m_pFont = m_pDevice->getGUIEnvironment()->getBuiltInFont();
        assert(m_pFont);
        if(!m_pFont)
            THROW_EXCEPTION("Failed to get Irrlicht builtin font");
        m_pFont->grab();
    }
    
    // Create console
    m_pConsole = new CClientConsole(m_pDevice->getGUIEnvironment());
    m_pConsole->SetFont(m_pFont);
    CLogger::GetInst().SetConsole(m_pConsole);
    
    // Create game
    m_pGame = new CGame(m_pConsole, m_pDevice);
    m_pGame->InitVfs();
    m_pGame->LoadTables();
    m_pGame->SetCamera(m_pCamera);
    
    m_pHud = new CHud(m_pGame);
    if(m_pFontFace)
    {
        gui::CGUITTFont *pHudFont = new gui::CGUITTFont(m_pDevice->getGUIEnvironment());
        pHudFont->attach(m_pFontFace, 18);
        m_pHud->SetFont(pHudFont);
        pHudFont->drop();
    }
    
    m_pWeaponSel = new CWeaponSelection(m_pGame);
    
    m_pConsole->Print("Game initialized!\n");
}

bool CClientApp::OnEvent(const SEvent &Event)
{
    // First let console process the event
    if(m_pConsole && m_pConsole->OnEvent(Event))
        return true;
    
    // Lets weapon selection handle the event
    if(m_pWeaponSel && m_pWeaponSel->OnEvent(Event))
        return true;
    
    // Let camera handle the event
    if(m_pCamera && m_pCamera->OnEvent(Event))
        return true;
    
    // Lets entity controller process the event
    if(m_pEntityController && m_pEntityController->OnEvent(Event))
        return true;
    
    // Remember whether each key is down or up
    if(Event.EventType == EET_KEY_INPUT_EVENT && Event.KeyInput.PressedDown)
    {
        if(Event.KeyInput.Key == 'C')
        {
            SwitchCamera();
            return true;
        }
        else if(Event.KeyInput.Key == KEY_ESCAPE)
            m_bQuit = true;
    }
    
    return false;
}

void CClientApp::Run()
{
    // Create labels for debug data
    gui::IGUIStaticText *DbgLabels[8];
    unsigned cyFont = m_pFont->getDimension(L"").Height;
    for(unsigned i = 0; i < COUNTOF(DbgLabels); ++i)
    {
        DbgLabels[i] = m_pDevice->getGUIEnvironment()->addStaticText(L"-", core::rect<s32>(10, 150 + i * cyFont, 200, 150 + (i + 1) * cyFont));
        DbgLabels[i]->setOverrideColor(video::SColor(128, 255, 255, 255));
        DbgLabels[i]->setOverrideFont(m_pFont);
    }
    
    while(m_pDevice->run())
	{
	    // Exit if Escape was pressed
        if(m_bQuit)
            break;
	    
	    // Calculate delta time
	    static CTimer FrameTimer;
        unsigned uDeltaTime;
        while(true)
        {
            // Don't allow 0 delta time
            uDeltaTime = FrameTimer.GetValue();
            if(uDeltaTime > 0)
                break;
            else
                m_pDevice->yield();
        }
        FrameTimer.Reset();
        
        // Display FPS
        wchar_t wszDbgBuf[256];
        float fFps = m_pDriver->getFPS();
        swprintf(wszDbgBuf, L"FPS: %.1f (frame took %.0f ms)", fFps, 1.0f / fFps * 1000);
        DbgLabels[0]->setText(wszDbgBuf);
        
        // Display level update time
        CTimer DbgTimer;
        m_pGame->GetLevel()->Update(uDeltaTime);
        swprintf(wszDbgBuf, L"Level update took %u ms", DbgTimer.GetValue());
        DbgLabels[1]->setText(wszDbgBuf);
        
        // Update local entity position
        if(m_pEntityController && m_pDevice->isWindowActive())
            m_pEntityController->Update(uDeltaTime);
        
        // Update camera
        m_pCamera->Update(uDeltaTime);
        
        // Prepare to scene rendering
        DbgTimer.Reset();
        const CLevelProperties *pLvlProp = m_pGame->GetLevel()->GetProperties();
		m_pDriver->beginScene(true, true, video::SColor(pLvlProp->FogColor));
		
        // Draw all scene objects
		m_pSceneMgr->drawAll();
        m_pGame->GetLevel()->DbgDraw();
        
        // Draw GUI
        m_pHud->Render();
        m_pWeaponSel->Render();
        m_pDevice->getGUIEnvironment()->drawAll();
        m_pConsole->Render();
        
        // End rendering
		m_pDriver->endScene();
		
		// Display rendering time
		swprintf(wszDbgBuf, L"Rendering took %u ms", DbgTimer.GetValue());
        DbgLabels[2]->setText(wszDbgBuf);
        
        // Display information about local entity
        if(m_pLocalEntity)
        {
            btVector3 vPos = m_pLocalEntity->GetPos();
            swprintf(wszDbgBuf, L"Pos: %.1f %.1f %.1f", vPos.x(), vPos.y(), vPos.z());
            DbgLabels[3]->setText(wszDbgBuf);
            
            swprintf(wszDbgBuf, L"Life: %.1f", m_pLocalEntity->GetLife());
            DbgLabels[4]->setText(wszDbgBuf);
            
            swprintf(wszDbgBuf, L"Armor: %.1f", m_pLocalEntity->GetArmor());
            DbgLabels[5]->setText(wszDbgBuf);
            
            swprintf(wszDbgBuf, L"State: %hs", m_pLocalEntity->GetState().GetName());
            DbgLabels[6]->setText(wszDbgBuf);
            
            swprintf(wszDbgBuf, L"Action: %hs", m_pLocalEntity->GetAction().GetName());
            DbgLabels[7]->setText(wszDbgBuf);
        }
	}
	
	// Unload level
	m_pGame->GetLevel()->Unload();
}

void CClientApp::LoadLevel(const char *pszName)
{
    // Load new level
    m_pGame->LoadLevel(pszName);
    
    // Recreate local entity (last one was deleted when level has been loaded)
    const SEntityClass *pEntityCls = m_pGame->GetEntitiesTbl()->Get(5u);
    m_pLocalEntity = new CEntity(m_pGame->GetLevel(), pEntityCls);
    btVector3 vPos = m_pGame->GetLevel()->GetStartPos();
    m_pLocalEntity->SetPos(vPos);
    const char *pszDefaultWeapon = m_pGame->GetGameTbl()->GetDefaultWeapon();
    const SWeaponClass *pWeaponCls = m_pGame->GetWeaponsTbl()->Get(pszDefaultWeapon);
    
    m_pLocalEntity->RemoveWeapons();
    m_pLocalEntity->AddWeapon(pWeaponCls, 64);
    
    m_pEntityController = new CEntityController(m_pLocalEntity, m_pDevice->getCursorControl());
    
    // Change camera to first person
    SetCameraType(false);
    
    m_pConsole->Print("Level %s has been loaded.\n", pszName);
}

void CClientApp::SetCameraType(bool bFreeCamera)
{
    m_bFreeCamera = bFreeCamera;
    delete m_pCamera;
    if(m_bFreeCamera)
        m_pCamera = new CFreeCamera(m_pSceneMgr, m_pLocalEntity, m_pDevice->getCursorControl());
    else
        m_pCamera = new CEntityCamera(m_pSceneMgr, m_pLocalEntity);
    m_pCamera->GetSceneNode()->setNearValue(0.1f);
    m_pCamera->GetSceneNode()->setFarValue(300.0f);
    m_pCamera->OnActivate();
    
    if(m_pGame)
        m_pGame->SetCamera(m_pCamera);
}
