/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/camera/CEntityCamera.cpp
*  PURPOSE:     Camera looking from head of entity
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CEntityCamera.h"
#include "CEntity.h"

using namespace std;
using namespace irr;

CEntityCamera::CEntityCamera(scene::ISceneManager *pSceneMgr, CEntity *pEntity):
    CCamera(pSceneMgr), m_pEntity(pEntity)
{
    assert(m_pEntity);
}

void CEntityCamera::Update(unsigned uDeltaTime)
{
    scene::ICameraSceneNode *pCamera = m_pSceneNode;
    
    // show reticle
    btVector3 vPos = m_pEntity->GetHeadPos();
    core::vector3df vCamPos(vPos.x(), vPos.y(), vPos.z());
    pCamera->setPosition(vCamPos);
    
    const btMatrix3x3 &matRot = m_pEntity->GetRotMatrix();
    btVector3 vTarget = vPos + matRot.getColumn(2);
    pCamera->setTarget(core::vector3df(vTarget.x(), vTarget.y(), vTarget.z()));
}

void CEntityCamera::OnActivate()
{
    if(m_pEntity && m_pEntity->GetSceneNode())
        m_pEntity->GetSceneNode()->setVisible(false);
}
