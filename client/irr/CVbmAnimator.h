/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CVbmAnimator.h
*  PURPOSE:     Texture animator for Irrlicht
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CIRRVBMANIMATOR_H
#define CIRRVBMANIMATOR_H

#include <irrlicht.h>
#include "CMaterial.h"

class CIrrVbmAnimator: public irr::scene::ISceneNodeAnimator
{
    public:
        CIrrVbmAnimator(CMaterial *pVbm, unsigned iMaterial = 0);
        
        virtual ~CIrrVbmAnimator();
        void animateNode(irr::scene::ISceneNode* node, irr::u32 uTimeMs);
        ISceneNodeAnimator *createClone(irr::scene::ISceneNode *pNode, irr::scene::ISceneManager *pNewManager);
    
    private:
        CMaterial *m_pMaterial;
        unsigned m_iMaterial;
};

#endif // CIRRVBMANIMATOR_H
