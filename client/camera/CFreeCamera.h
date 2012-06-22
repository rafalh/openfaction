/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/camera/CFreeCamera.h
*  PURPOSE:     Free camera implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFREECAMERA_H
#define CFREECAMERA_H

#include <irrlicht.h>
#include "CCamera.h"

class CEntity;

class CFreeCamera: public CCamera
{
    public:
        CFreeCamera(irr::scene::ISceneManager *pSceneMgr, CEntity *pEntity, irr::gui::ICursorControl *pCursorCtrl = NULL);
        ~CFreeCamera();
        void Update(unsigned uDeltaTime);
        bool OnEvent(const irr::SEvent &Event);
        void OnActivate();
    
    private:
        CEntity *m_pEntity;
        irr::gui::ICursorControl *m_pCursorCtrl;
        
        enum EControl
        {
            CTRL_FORWARD,
            CTRL_BACKWARD,
            CTRL_LEFT,
            CTRL_RIGHT,
            CTRL_FASTER,
            CTRL_MAX
        };
        bool m_Controls[CTRL_MAX];
        
        irr::core::position2df m_vCursorCenter;
        irr::core::position2df m_vCursorPos;
        float m_fYaw, m_fPitch;
        
        irr::core::vector3df m_vVelocity;
};

#endif // CFREECAMERA_H
