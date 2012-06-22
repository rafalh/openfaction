/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CHud.h
*  PURPOSE:     HUD implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CHUD_H
#define CHUD_H

#include <irrlicht.h>

class CGame;
class CEntity;
class CMaterial;

class CHud
{
    public:
        CHud(CGame *pGame);
        ~CHud();
        void Render();
        void SetEntity(CEntity *pEntity);
        
        void SetFont(irr::gui::IGUIFont *pFont)
        {
            if(m_pFont)
                m_pFont->drop();
            m_pFont = pFont;
            if(m_pFont)
                m_pFont->grab();
        }
    
    private:
        CGame *m_pGame;
        CEntity *m_pEntity;
        CMaterial *m_pReticleTexture;
        CMaterial *m_HealthTextures[11];
        CMaterial *m_EnviroTextures[11];
        CMaterial *m_pAmmoBarTex;
        CMaterial *m_pAmmoBarPowerTex;
        CMaterial *m_pAmmoSignalGreenTex;
        CMaterial *m_pAmmoSignalRedTex;
        irr::gui::IGUIFont *m_pFont;
        
        void RenderReticle();
        void RenderHealthArmor();
        void RenderAmmo();
};

#endif // CHUD_H
