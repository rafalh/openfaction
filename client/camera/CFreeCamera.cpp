/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/camera/CFreeCamera.cpp
*  PURPOSE:     Free camera implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include <algorithm>
#include "CFreeCamera.h"
#include "CEntity.h"

using namespace std;
using namespace irr;

CFreeCamera::CFreeCamera(irr::scene::ISceneManager *pSceneMgr, CEntity *pEntity, irr::gui::ICursorControl *pCursorCtrl):
    CCamera(pSceneMgr), m_pEntity(pEntity), m_fYaw(0.0f), m_fPitch(0.0f)
{
    m_pCursorCtrl = pCursorCtrl;
    if(m_pCursorCtrl)
        m_pCursorCtrl->grab();
    
    for(int Ctrl = 0; Ctrl != CTRL_MAX; ++Ctrl)
        m_Controls[Ctrl] = false;
}

CFreeCamera::~CFreeCamera()
{
    if(m_pCursorCtrl)
        m_pCursorCtrl->drop();
    m_pCursorCtrl = NULL;
}

void CFreeCamera::Update(unsigned uDeltaTime)
{
    scene::ICameraSceneNode *pCamera = m_pSceneNode;
    
    // Check if dt is not 0 and we are animating active camera
    if(!uDeltaTime || pCamera->getSceneManager()->getActiveCamera() != pCamera)
        return;
    
    // Calculate some useful vectors
    core::vector3df vPos = pCamera->getPosition();
    const core::vector3df vForward = (pCamera->getTarget() - pCamera->getPosition()).normalize();
    const core::vector3df &vUp = pCamera->getUpVector();
    const core::vector3df vRight = vForward.crossProduct(vUp);
    float fSpeed = uDeltaTime / 100.0f;
    
    // Shift makes camera faster
    if(m_Controls[CTRL_FASTER])
        fSpeed *= 5.0f;
    
    // Calculate direction of movement
    core::vector3df vMovement(0.0f, 0.0f, 0.0f);
    if(m_Controls[CTRL_FORWARD])
        vMovement += vForward * fSpeed;
    if(m_Controls[CTRL_BACKWARD])
        vMovement -= vForward * fSpeed;
    if(m_Controls[CTRL_LEFT])
        vMovement += vRight * fSpeed;
    if(m_Controls[CTRL_RIGHT])
        vMovement -= vRight * fSpeed;
    
    // update camera velocity
    float fFactor = min(uDeltaTime / 200.0f, 1.0f);
    m_vVelocity = vMovement * fFactor + m_vVelocity * (1.0f - fFactor);
    vPos += m_vVelocity;
    pCamera->setPosition(vPos);
    
    // Update pitch and yaw
    if(m_vCursorPos != m_vCursorCenter)
    {
        core::vector2df vOffset = m_vCursorPos - m_vCursorCenter;
        
        m_fYaw = fmod(m_fYaw + vOffset.X, 2.0f * (float)M_PI);
        
        const float fPitchMax = (float)M_PI_2 - 0.1f;
        m_fPitch -= vOffset.Y;
        if(m_fPitch > fPitchMax)
            m_fPitch = fPitchMax;
        else if(m_fPitch < -fPitchMax)
            m_fPitch = -fPitchMax;
        
        m_pCursorCtrl->setPosition(0.5f, 0.5f);
        m_vCursorCenter = m_pCursorCtrl->getRelativePosition();
    }
    
    // Set camera target
    core::vector3df vTarget(sinf(m_fYaw) * cosf(m_fPitch), sinf(m_fPitch), cosf(m_fYaw) * cosf(m_fPitch));
    vTarget += vPos;
    pCamera->setTarget(vTarget);
}

bool CFreeCamera::OnEvent(const SEvent &Event)
{
    if(Event.EventType == EET_KEY_INPUT_EVENT)
    {
        // Check control type
        EControl Ctrl;
        switch(Event.KeyInput.Key)
        {
            case KEY_UP:
            case 'W':
                Ctrl = CTRL_FORWARD;
                break;
            case KEY_DOWN:
            case 'S':
                Ctrl = CTRL_BACKWARD;
                break;
            case KEY_LEFT:
            case 'A':
                Ctrl = CTRL_LEFT;
                break;
            case KEY_RIGHT:
            case 'D':
                Ctrl = CTRL_RIGHT;
                break;
            case KEY_LSHIFT:
            case KEY_RSHIFT:
                Ctrl = CTRL_FASTER;
                break;
            default:
                return false;
        }
        
        // Save control state
        m_Controls[Ctrl] = Event.KeyInput.PressedDown;
        return true;
    }
    else if(Event.EventType == EET_MOUSE_INPUT_EVENT && Event.MouseInput.Event == EMIE_MOUSE_MOVED && m_pCursorCtrl)
    {
        // Save cursor position for later use
        m_vCursorPos = m_pCursorCtrl->getRelativePosition();
        return true;
    }
    
    return false;
}

void CFreeCamera::OnActivate()
{
    if(m_pEntity)
    {
        btVector3 vPos = m_pEntity->GetPos();
        m_pSceneNode->setPosition(core::vector3df(vPos.x(), vPos.y(), vPos.z()));
    }
    
    if(m_pEntity && m_pEntity->GetSceneNode())
        m_pEntity->GetSceneNode()->setVisible(true);
    
    if(m_pCursorCtrl)
    {
        m_pCursorCtrl->setPosition(0.5f, 0.5f);
        m_vCursorCenter = m_pCursorCtrl->getRelativePosition();
    }
}
