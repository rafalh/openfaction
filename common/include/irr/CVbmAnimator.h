/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CVbmAnimator.h
*  PURPOSE:     Texture animator for Irrlicht
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#pragma once

#include <irrlicht.h>
#include "CMultiTexture.h"

class CIrrVbmAnimator: public irr::scene::ISceneNodeAnimator
{
    public:
        CIrrVbmAnimator(CMultiTexture *pVbm, unsigned MaterialIndex = 0);
        
        virtual ~CIrrVbmAnimator();
        void animateNode(irr::scene::ISceneNode* node, irr::u32 uTimeMs);
        ISceneNodeAnimator *createClone(irr::scene::ISceneNode *pNode, irr::scene::ISceneManager *pNewManager);
    
    private:
        CMultiTexture *m_pMultiTexture;
        unsigned m_iMaterial;
};
