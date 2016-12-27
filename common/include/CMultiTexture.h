/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMaterial.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#pragma once

#include <vector>
#include <irrlicht.h>
#include "util/CInputBinaryStream.h"
#include "util/CSharedObject.h"

class CTextureMgr;

class CMultiTexture: public CSharedObject
{
    public:
        irr::scene::ISceneNodeAnimator *CreateAnimator(unsigned iNodeMaterial);
        
        irr::video::ITexture *GetFrame(unsigned i)
        {
            return i < m_Frames.size() ? m_Frames[i] : NULL;
        }
        
        unsigned GetFramesCount() const
        {
            return m_Frames.size();
        }
        
        unsigned GetFps() const
        {
            return m_Fps;
        }
        
        bool IsAnimated() const
        {
            return m_bAnimated;
        }
        
        bool HasAlpha() const
        {
            return m_bAlpha;
        }

        bool IsInvisible() const
        {
            return m_bInvisible;
        }
    
    private:
        CTextureMgr *m_pTextureMgr;
        std::vector<irr::video::ITexture*> m_Frames;
        unsigned m_Fps;
        bool m_bAnimated, m_bAlpha;
        bool m_bInvisible;
        
        static unsigned m_UniqueId;
        
        CMultiTexture(CTextureMgr *pTextureMgr, const CString &strFilename);
        ~CMultiTexture();
        bool LoadVbm(CInputBinaryStream &Stream);
    
    friend class CTextureMgr;
};
