/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CEntityControler.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CEntityControler.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "CEntity.h"
#include "CLevel.h"
#include "CGame.h"
#include "CConsole.h"

using namespace std;
using namespace irr;

#define USE_FORCES 0

CEntityController::CEntityController(CEntity *pEntity, irr::gui::ICursorControl *pCursorCtrl):
    m_fYaw(0.0f), m_fPitch(0.0f), m_pEntity(pEntity)
{
    m_pCursorCtrl = pCursorCtrl;
    if(m_pCursorCtrl)
        m_pCursorCtrl->grab();
    
    for(int Ctrl = 0; Ctrl != CTRL_MAX; ++Ctrl)
        m_Controls[Ctrl] = false;
}

CEntityController::~CEntityController()
{
    if(m_pCursorCtrl)
        m_pCursorCtrl->drop();
    m_pCursorCtrl = NULL;
}

void CEntityController::Update(unsigned uDeltaTime)
{
    // Update pitch and yaw
    if(m_vCursorPos != m_vCursorCenter)
    {
        core::vector2df vOffset = m_vCursorPos - m_vCursorCenter;
        
        m_fYaw = fmod(m_fYaw + vOffset.X, 2.0f * M_PI);
        
        const float fPitchMax = M_PI_2 - 0.1f;
        m_fPitch += vOffset.Y;
        if(m_fPitch > fPitchMax)
            m_fPitch = fPitchMax;
        else if(m_fPitch < -fPitchMax)
            m_fPitch = -fPitchMax;
        
        m_pCursorCtrl->setPosition(0.5f, 0.5f);
        m_vCursorCenter = m_pCursorCtrl->getRelativePosition();
    }
    
    btMatrix3x3 matRot = m_pEntity->GetRotMatrix();
    //matRot.setEulerYPR(m_fYaw, m_fPitch, 0.0f);
    matRot.setEulerZYX(m_fPitch, m_fYaw, 0.0f);
    m_pEntity->SetRotMatrix(matRot);
    
    btVector3 vDir(0.0f, 0.0f, 0.0f);
    if(m_Controls[CTRL_FORWARD])
        vDir += matRot.getColumn(2);
    if(m_Controls[CTRL_BACKWARD])
        vDir += -matRot.getColumn(2);
    if(m_Controls[CTRL_RIGHT])
        vDir += matRot.getColumn(0);
    if(m_Controls[CTRL_LEFT])
        vDir += -matRot.getColumn(0);
    
    vDir[1] = 0;  // No Y movement
    
    //if(vDir.length2() > 0.0f)
    //    printf("Dir %f %f %f\n", vDir.x(), vDir.y(), vDir.z());
    
    if(!m_pEntity->GetCharacter())
    {
        btRigidBody *pBody = btRigidBody::upcast(m_pEntity->GetColObject());
        //vDir = btVector3(1.0f, 0.0f, 0.0f);
        pBody->activate();
#if USE_FORCES
        if(vDir.length2() > 0.0f)
        {
            btVector3 vVel = pBody->getLinearVelocity();
            if(vVel.length2() < 100.0f)
                pBody->applyCentralForce(vDir * 5000.0f);
        }
#else
        btVector3 vVel = pBody->getLinearVelocity();
        vVel[0] = vDir[0] * 10.0f;
        vVel[2] = vDir[2] * 10.0f;
        pBody->setLinearVelocity(vVel);
#endif
        
        btTransform Transform = pBody->getCenterOfMassTransform();
        //pBody->getMotionState()->getWorldTransform(Transform);
        Transform.setRotation(btQuaternion(btVector3(0.0, 1.0, 0.0), m_fYaw));
        pBody->getMotionState()->setWorldTransform(Transform);
        pBody->setCenterOfMassTransform(Transform);
    }
    else
        m_pEntity->GetCharacter()->setWalkDirection(vDir * uDeltaTime / 200.0f);
}

void CEntityController::Jump()
{
    if(m_pEntity->GetCharacter())
        m_pEntity->GetCharacter()->jump();
    else
    {
        btRigidBody *pBody = btRigidBody::upcast(m_pEntity->GetColObject());
        // FIXME: is on ground?
#if USE_FORCES
        pBody->applyCentralImpulse(btVector3(0.0f, 500.0f, 0.0f));
#else
        btVector3 vVel = pBody->getLinearVelocity();
        vVel[1] = max(vVel[1], 5.0f);
        pBody->setLinearVelocity(vVel);
#endif
    }
}

bool CEntityController::OnEvent(const SEvent &Event)
{
    if(Event.EventType == EET_KEY_INPUT_EVENT)
    {
        // Check input key type
        switch(Event.KeyInput.Key)
        {
            case KEY_UP:
            case 'W':
                m_Controls[CTRL_FORWARD] = Event.KeyInput.PressedDown;
                break;
            case KEY_DOWN:
            case 'S':
                m_Controls[CTRL_BACKWARD] = Event.KeyInput.PressedDown;
                break;
            case KEY_LEFT:
            case 'A':
                m_Controls[CTRL_LEFT] = Event.KeyInput.PressedDown;
                break;
            case KEY_RIGHT:
            case 'D':
                m_Controls[CTRL_RIGHT] = Event.KeyInput.PressedDown;
                break;
            case 'R':
                if(Event.KeyInput.PressedDown)
                    if(!m_pEntity->ReloadWeapon())
                        m_pEntity->GetLevel()->GetGame()->GetConsole()->DbgPrint("Failed to reload");
                break;
            case KEY_SPACE:
                if(Event.KeyInput.PressedDown)
                    Jump();
                break;
            default:
                return false;
        }
        
        return false; //true;
    }
    else if(Event.EventType == EET_MOUSE_INPUT_EVENT && Event.MouseInput.Event == EMIE_MOUSE_MOVED && m_pCursorCtrl)
    {
        // Save cursor position for later use
        m_vCursorPos = m_pCursorCtrl->getRelativePosition();
        return false; //true;
    }
    else if(Event.EventType == EET_MOUSE_INPUT_EVENT && Event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
    {
        if(!m_pEntity->Shoot(false, false))
            m_pEntity->GetLevel()->GetGame()->GetConsole()->DbgPrint("Failed to shoot");
    }
        
    
    return false;
}
