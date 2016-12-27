/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CWeaponSelection.cpp
*  PURPOSE:     Weapon selection GUI
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CWeaponSelection.h"
#include "util/utils.h"
#include "CTextureMgr.h"
#include "CWeapon.h"
#include "CEntity.h"
#include "CGame.h"
#include "CConsole.h"
#include "tables/CStringsTable.h"
#include "camera/CCamera.h"

using namespace std;
using namespace irr;

const unsigned CWeaponSelection::VISIBLE_TIME = 3000;

CWeaponSelection::CWeaponSelection(CGame *pGame):
    m_pGame(pGame),
    m_WeaponType(WT_SEMI_AUTO), m_iWeaponCls(0), m_bVisible(false)
{
    for(unsigned i = 0; i < COUNTOF(m_Textures); ++i)
        m_Textures[i] = 0;
}

CWeaponSelection::~CWeaponSelection()
{
    for(unsigned i = 0; i < COUNTOF(m_Textures); ++i)
        if(m_Textures[i])
            m_Textures[i]->Release();
}

void CWeaponSelection::Render()
{
    CEntity *pEntity = dynamic_cast<CEntity*>(m_pGame->GetCamera()->GetTarget());
    if(!m_bVisible || !pEntity)
        return;
    
    if(m_Timer.GetValue() > VISIBLE_TIME)
    {
        m_bVisible = false;
        return;
    }
    
    const core::dimension2du &ScrSize = m_pGame->GetVideoDriver()->getScreenSize();
    wchar_t wszBuf[64];
    
    unsigned w = 128, h = 0;
    unsigned x = ScrSize.Width - w - 30;
    unsigned y = ScrSize.Height * 2 / 5;
    
    gui::IGUIFont *pFont = m_pGame->GetGuiEnv()->getBuiltInFont();
    h = pFont->getDimension(L"").Height;
    const char *pszWeaponType = m_pGame->GetTables()->strings()->Get(779 + (int)m_WeaponType);
    swprintf(wszBuf, L"%hs", pszWeaponType);
    pFont->draw(wszBuf, core::recti(x, y, x + w, y + h), video::SColor(255, 0, 255, 0), true, true);
    y += h;
    
    for(unsigned i = 0; i < 4; ++i)
    {
        unsigned x2 = x + (w - 4*20 - 3*5)/2 + i * 25;
        core::recti rc(x2, y, x2 + 20, y + 20);
        m_pGame->GetVideoDriver()->draw2DRectangle(video::SColor(128, 0, 0, 0), rc);
        
        swprintf(wszBuf, L"%u", i + 1);
        pFont->draw(wszBuf, rc, video::SColor(255, 0, 255, 0), true, true);
        
        if(m_WeaponType == (EWeaponType)i)
            m_pGame->GetVideoDriver()->draw2DRectangleOutline(rc, video::SColor(255, 255, 255, 0));
    }
    
    y += 25;
    
    unsigned i = 0;
    const SWeaponClass *pWeaponCls = NULL;
    while(true)
    {
        CWeapon *pWeapon = FindWeaponFromType(pEntity, m_WeaponType, pWeaponCls ? pWeaponCls->nId + 1 : 0);
        if(!pWeapon)
            break;
        
        pWeaponCls = pWeapon->GetClass();
        
        if(!m_Textures[pWeaponCls->nId])
            m_Textures[pWeaponCls->nId] = m_pGame->GetTextureMgr()->Load(pWeaponCls->strWeaponIcon);
        
        CMultiTexture *pTex = m_Textures[pWeaponCls->nId];
        if(pTex)
        {
            const core::dimension2du &ImgSize = pTex->GetFrame(0)->getSize();
            
            core::recti SrcRect(0, 0, ImgSize.Width, ImgSize.Height);
            core::position2di DestPos(ScrSize.Width - ImgSize.Width - 30, y);
            
            video::SColor clrWeapon;
            if(pWeapon->GetAmmo() > 0 || pEntity->GetAmmo(pWeaponCls->pAmmoType) > 0)
                clrWeapon.set(255, 128, 255, 0);
            else
                clrWeapon.set(255, 255, 128, 128);
            
            m_pGame->GetVideoDriver()->draw2DImage(pTex->GetFrame(0), DestPos, SrcRect, 0, clrWeapon, false);
            if(m_iWeaponCls == pWeaponCls->nId)
            {
                core::recti BorderRect(DestPos.X, DestPos.Y, DestPos.X + ImgSize.Width, DestPos.Y + ImgSize.Height - 1);
                m_pGame->GetVideoDriver()->draw2DRectangleOutline(BorderRect, video::SColor(255, 255, 255, 0));
            }
            
            y += ImgSize.Height;
            ++i;
        }
    }
    
    y += 50;
    pWeaponCls = m_pGame->GetTables()->weapons()->Get(m_iWeaponCls);
    swprintf(wszBuf, L"%hs", pWeaponCls->strDisplayName.c_str());
    pFont->draw(wszBuf, core::recti(x, y, x + w, y), video::SColor(255, 0, 255, 0), true);
}

bool CWeaponSelection::OnEvent(const SEvent &Event)
{
    if(Event.EventType == EET_KEY_INPUT_EVENT && Event.KeyInput.PressedDown)
    {
        if(Event.KeyInput.Key >= '1' && Event.KeyInput.Key <= '4')
        {
            EWeaponType NewWeaponType = (EWeaponType)(Event.KeyInput.Key - '1');
            CEntity *pEntity = dynamic_cast<CEntity*>(m_pGame->GetCamera()->GetTarget());
            
            if(m_bVisible && NewWeaponType == m_WeaponType)
            {
                CWeapon *pWeapon = FindWeaponFromType(pEntity, m_WeaponType, m_iWeaponCls + 1);
                if(!pWeapon)
                    pWeapon = FindWeaponFromType(pEntity, m_WeaponType);
                
                assert(pWeapon);
                m_iWeaponCls = pWeapon->GetClass()->nId;
                m_Timer.Reset();
            }
            else
            {
                CWeapon *pWeapon = FindWeaponFromType(pEntity, NewWeaponType);
                if(!pWeapon)
                    return false;
                
                m_WeaponType = NewWeaponType;
                m_iWeaponCls = pWeapon->GetClass()->nId;
                m_Timer.Reset();
                m_bVisible = true;
            }
            
            return true;
        }
    }
    else if(Event.EventType == EET_MOUSE_INPUT_EVENT && Event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN && m_bVisible)
    {
        const SWeaponClass *pWeaponCls = m_pGame->GetTables()->weapons()->Get(m_iWeaponCls);
        CEntity *pEntity = dynamic_cast<CEntity*>(m_pGame->GetCamera()->GetTarget());
        if(pEntity)
            pEntity->SwitchWeapon(pWeaponCls);
        m_bVisible = false;
        return true;
    }
    else if(Event.EventType == EET_MOUSE_INPUT_EVENT && Event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
    {
        m_bVisible = false;
        return true;
    }
    
    return false;
}

CWeapon *CWeaponSelection::FindWeaponFromType(CEntity *pEntity, EWeaponType Type, unsigned iHint)
{
    unsigned i = iHint;
    
    if(!pEntity)
        return NULL;
    
    while(true)
    {
        const SWeaponClass *pWeaponCls = m_pGame->GetTables()->weapons()->Get(i++);
        if(!pWeaponCls)
            break;
        
        if(pWeaponCls->WeaponType != Type || pWeaponCls->strWeaponIcon.empty())
            continue;
        
        CWeapon *pWeapon = pEntity->GetWeapon(pWeaponCls);
        if(!pWeapon)
            continue;
        
        return pWeapon;
    }
    
    return NULL;
}
