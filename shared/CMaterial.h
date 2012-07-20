/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMaterial.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CVBM_H
#define CVBM_H

#include <vector>
#ifdef OF_CLIENT
# include <irrlicht.h>
#endif // OF_CLIENT
#include "CInputBinaryStream.h"
#include "CSharedObject.h"

class CMaterialsMgr;

class CMaterial: public CSharedObject
{
    public:
#ifdef OF_CLIENT
        irr::scene::ISceneNodeAnimator *CreateAnimator(unsigned iNodeMaterial);
#endif // OF_CLIENT
        
#ifdef OF_CLIENT
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
#endif // OF_CLIENT

        bool IsInvisible() const
        {
            return m_bInvisible;
        }
    
    private:
        CMaterialsMgr *m_pMaterialsMgr;
#ifdef OF_CLIENT
        std::vector<irr::video::ITexture*> m_Frames;
        unsigned m_Fps;
        bool m_bAnimated, m_bAlpha;
#endif // OF_CLIENT
        bool m_bInvisible;
        
        static unsigned m_UniqueId;
        
        CMaterial(CMaterialsMgr *pMaterialsMgr, const std::string &strFilename);
        ~CMaterial();
#ifdef OF_CLIENT
        bool LoadVbm(CInputBinaryStream &Stream);
#endif // OF_CLIENT
    
    friend class CMaterialsMgr;
};

#endif // CVBM_H
