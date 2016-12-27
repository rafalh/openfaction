/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMaterialsMgr.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#pragma once

#include "util/CString.h"
#include <map>
#include <irrlicht.h>
#include "CMultiTexture.h"

class CGame;

class CTextureMgr
{
    public:
        CTextureMgr(irr::video::IVideoDriver *videoDriver):
            m_videoDriver(videoDriver) {}
        
        ~CTextureMgr();
        CMultiTexture *Load(const CString &strFilename);
        
        irr::video::IVideoDriver *GetVideoDriver() const
        {
            return m_videoDriver;
        }
        
    private:
        std::map<CString, CMultiTexture*> m_Materials;
        irr::video::IVideoDriver *m_videoDriver;
        
        void Remove(CMultiTexture *pMaterial);
        
    friend class CMultiTexture;
};
