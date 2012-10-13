/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CHud.cpp
*  PURPOSE:     HUD implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CHud.h"
#include "CMaterialsMgr.h"
#include "CException.h"
#include "CGame.h"
#include "CEntity.h"
#include "CWeapon.h"
#include "CString.h"

using namespace std;
using namespace irr;

CHud::CHud(CGame *pGame):
    m_pGame(pGame),
    m_pEntity(0), m_pReticleTexture(NULL),
    m_pFont(NULL)
{
    for(unsigned i = 0; i <= 10; ++i)
    {
        CString strFilename;
        strFilename.Format("health%u_0.tga", i * 10);
        m_HealthTextures[i] = m_pGame->GetMaterialsMgr()->Load(strFilename);
    }
    
    for(unsigned i = 0; i <= 10; ++i)
    {
        CString strFilename;
        strFilename.Format("enviro%u_0.tga", i * 10);
        m_EnviroTextures[i] = m_pGame->GetMaterialsMgr()->Load(strFilename);
    }
    
    m_pAmmoBarTex = m_pGame->GetMaterialsMgr()->Load("ammo_bar_0.tga");
    m_pAmmoBarPowerTex = m_pGame->GetMaterialsMgr()->Load("ammo_bar_power_0.tga");
    m_pAmmoSignalGreenTex = m_pGame->GetMaterialsMgr()->Load("ammo_signal_green_0.tga");
    m_pAmmoSignalRedTex = m_pGame->GetMaterialsMgr()->Load("ammo_signal_red_0.tga");
}

CHud::~CHud()
{
    if(m_pReticleTexture)
        m_pReticleTexture->Release();
    
    for(unsigned i = 0; i <= 10; ++i)
        m_HealthTextures[i]->Release();
    
    for(unsigned i = 0; i <= 10; ++i)
        m_EnviroTextures[i]->Release();
    
    if(m_pFont)
        m_pFont->drop();
    if(m_pAmmoBarTex)
        m_pAmmoBarTex->Release();
    if(m_pAmmoBarPowerTex)
        m_pAmmoBarPowerTex->Release();
    if(m_pAmmoSignalGreenTex)
        m_pAmmoSignalGreenTex->Release();
    if(m_pAmmoSignalRedTex)
        m_pAmmoSignalRedTex->Release();
}

void CHud::Render()
{
    if(!m_pEntity || !m_pEntity->IsAlive())
        return;
    
    RenderReticle();
    RenderHealthArmor();
    RenderAmmo();
}

void CHud::SetEntity(CEntity *pEntity)
{
    m_pEntity = pEntity;
    
    if(m_pReticleTexture)
        m_pReticleTexture->Release();
    
    m_pReticleTexture = m_pGame->GetMaterialsMgr()->Load("reticle_0.tga");
}

void CHud::RenderReticle()
{
    if(!m_pReticleTexture)
        return;
    
    const core::dimension2du &ScrSize = m_pGame->GetVideoDriver()->getScreenSize();
    
    const core::dimension2du &ImgSize = m_pReticleTexture->GetFrame(0)->getSize();
    core::position2di DestPos((ScrSize.Width - ImgSize.Width) / 2, (ScrSize.Height - ImgSize.Height) / 2);
    core::recti SrcRect(0, 0, ImgSize.Width, ImgSize.Height);
    
    m_pGame->GetVideoDriver()->draw2DImage(m_pReticleTexture->GetFrame(0), DestPos, SrcRect, 0, video::SColor(255, 255, 255, 255), true);
}

void CHud::RenderHealthArmor()
{
    unsigned iHealthTex = static_cast<int>(m_pEntity->GetLife()) / 10;
    if(iHealthTex > 10)
        iHealthTex = 10;
    else if(iHealthTex < 0)
        iHealthTex = 0;
    
    CMaterial *pHealthTex = m_HealthTextures[iHealthTex];
    
    const core::dimension2du &HealthImgSize = pHealthTex->GetFrame(0)->getSize();
    core::position2di HealthDestPos(30, 30);
    core::recti HealthSrcRect(0, 0, HealthImgSize.Width, HealthImgSize.Height);
    
    m_pGame->GetVideoDriver()->draw2DImage(pHealthTex->GetFrame(0), HealthDestPos, HealthSrcRect, 0, video::SColor(96, 255, 255, 255), true);
    
    unsigned iEnviroTex = static_cast<int>(m_pEntity->GetArmor()) / 10;
    if(iEnviroTex > 10)
        iEnviroTex = 10;
    else if(iEnviroTex < 0)
        iEnviroTex = 0;
    
    CMaterial *pEnviroTex = m_EnviroTextures[iEnviroTex];
    
    const core::dimension2du &EnviroImgSize = pEnviroTex->GetFrame(0)->getSize();
    core::position2di EnviroDestPos(10, 10);
    core::recti EnviroSrcRect(0, 0, EnviroImgSize.Width, EnviroImgSize.Height);
    
    m_pGame->GetVideoDriver()->draw2DImage(pEnviroTex->GetFrame(0), EnviroDestPos, EnviroSrcRect, 0, video::SColor(96, 255, 255, 255), true);
    
    gui::IGUIFont *pFont = m_pFont ? m_pFont : m_pGame->GetGuiEnv()->getBuiltInFont();
    wchar_t wszBuf[64];
    
    core::recti HealthTextRect(70, 55, 100, 80);
    swprintf(wszBuf, L"%.0f", m_pEntity->GetLife());
    pFont->draw(wszBuf, HealthTextRect, video::SColor(255, 255, 255, 0), true, true);
    
    core::recti EnviroTextRect(110, 55, 140, 80);
    swprintf(wszBuf, L"%.0f", m_pEntity->GetArmor());
    pFont->draw(wszBuf, EnviroTextRect, video::SColor(255, 255, 255, 0), true, true);
}

void CHud::RenderAmmo()
{
    if(!m_pEntity->GetWeapon())
        return;
    
    bool bHasClip = (m_pEntity->GetWeapon()->GetClass()->nClipSize > 0);
    unsigned cClipAmmo = m_pEntity->GetWeapon()->GetAmmo();
    unsigned cAmmo = m_pEntity->GetAmmo(m_pEntity->GetWeapon()->GetClass()->pAmmoType);
    bool bHasAmmo = cClipAmmo > 0 || (!bHasClip && cAmmo > 0);
    
    const core::dimension2du &ScrSize = m_pGame->GetVideoDriver()->getScreenSize();
    
    const core::dimension2du &AmmoBarSize = m_pAmmoBarTex->GetFrame(0)->getSize();
    core::position2di AmmoBarDestPos(ScrSize.Width - AmmoBarSize.Width - 30, 30);
    core::recti AmmoBarSrcRect(0, 0, AmmoBarSize.Width, AmmoBarSize.Height);
    
    m_pGame->GetVideoDriver()->draw2DImage(m_pAmmoBarTex->GetFrame(0), AmmoBarDestPos, AmmoBarSrcRect, 0, video::SColor(96, 255, 255, 255), true);
    
    CMaterial *pSignalTex = bHasAmmo ? m_pAmmoSignalGreenTex : m_pAmmoSignalRedTex;
    const core::dimension2du &AmmoSignalSize = pSignalTex->GetFrame(0)->getSize();
    core::position2di AmmoSignalDestPos(ScrSize.Width - AmmoBarSize.Width - 30 - AmmoSignalSize.Width, 30);
    core::recti AmmoSignalSrcRect(0, 0, AmmoSignalSize.Width, AmmoSignalSize.Height);
    
    m_pGame->GetVideoDriver()->draw2DImage(pSignalTex->GetFrame(0), AmmoSignalDestPos, AmmoSignalSrcRect, 0, video::SColor(96, 255, 255, 255), true);
    
    gui::IGUIFont *pFont = m_pFont ? m_pFont : m_pGame->GetGuiEnv()->getBuiltInFont();
    wchar_t wszBuf[64];
    
    core::recti AmmoInClipRect(AmmoBarDestPos.X, AmmoBarDestPos.Y, AmmoBarDestPos.X + AmmoBarSize.Width*2/5, AmmoBarDestPos.Y + AmmoBarSize.Height);
    swprintf(wszBuf, L"%u", cClipAmmo);
    pFont->draw(wszBuf, AmmoInClipRect, video::SColor(255, 255, 255, 0), true, true);
    
    core::recti TotalAmmoRect(AmmoBarDestPos.X + AmmoBarSize.Width*2/5, AmmoBarDestPos.Y, AmmoBarDestPos.X + AmmoBarSize.Width, AmmoBarDestPos.Y + AmmoBarSize.Height);
    swprintf(wszBuf, L"%u", cAmmo);
    pFont->draw(wszBuf, TotalAmmoRect, video::SColor(255, 255, 255, 0), true, true);
}
