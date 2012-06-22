/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CEntityControler.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CENTITYHUMANANIMATOR_H
#define CENTITYHUMANANIMATOR_H

#include <irrlicht.h>

class CEntity;

class CEntityController: public irr::IEventReceiver
{
    public:
        CEntityController(CEntity *pEntity, irr::gui::ICursorControl *pCursorCtrl = NULL);
        ~CEntityController();
        void Update(unsigned uDeltaTime);
        bool OnEvent(const irr::SEvent &Event);
        void Jump();
    
    private:
        irr::gui::ICursorControl *m_pCursorCtrl;
        
        enum EControl
        {
            CTRL_FORWARD,
            CTRL_BACKWARD,
            CTRL_LEFT,
            CTRL_RIGHT,
            CTRL_MAX
        };
        bool m_Controls[CTRL_MAX];
        
        irr::core::position2df m_vCursorCenter;
        irr::core::position2df m_vCursorPos;
        float m_fYaw, m_fPitch;
        
        CEntity *m_pEntity;
};

#endif // CENTITYHUMANANIMATOR_H
