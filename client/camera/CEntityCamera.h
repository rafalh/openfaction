/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/camera/CEntityCamera.h
*  PURPOSE:     Camera looking from head of entity
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CENTITYCAMERA_H
#define CENTITYCAMERA_H

#include <irrlicht.h>
#include "CCamera.h"

class CEntity;

class CEntityCamera: public CCamera
{
    public:
        CEntityCamera(irr::scene::ISceneManager *pSceneMgr, CEntity *pEntity);
        void Update(unsigned uDeltaTime);
        void OnActivate();
    
    private:
        CEntity *m_pEntity;
};

#endif // CENTITYCAMERA_H
