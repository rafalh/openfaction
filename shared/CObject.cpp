/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CObject.cpp
*  PURPOSE:     Base class for all physical objects (level objects with known position)
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CObject.h"
#include "CLevel.h"

#ifdef OF_CLIENT
using namespace irr;
#endif

CObject::CObject(EElementType Type, CLevel *pLevel, unsigned nUid):
    CElement(Type, pLevel, nUid), m_pColObj(NULL),
#ifdef OF_CLIENT
    m_pSceneNode(NULL),
#endif
    m_bTransformChanged(true), m_bIsInWorld(false),
    m_iColGroup(0), m_iColMask(COL_NONE) {}

CObject::~CObject()
{
    RemoveFromWorld();
    
    if(m_pColObj)
        delete m_pColObj;
    m_pColObj = NULL;
    
#ifdef OF_CLIENT
    if(m_pSceneNode)
        m_pSceneNode->remove();
    m_pSceneNode = NULL;
#endif // OF_CLIENT
}

void CObject::Update(unsigned nDeltaTime)
{
#ifdef OF_CLIENT
    if(m_pSceneNode && m_pColObj && m_bTransformChanged)
    {
        btVector3 BtPos = GetPos();
        core::vector3df IrrPos(BtPos[0], BtPos[1], BtPos[2]);
        m_pSceneNode->setPosition(IrrPos);
        
        btScalar fRotX, fRotY, fRotZ;
        m_pColObj->getWorldTransform().getBasis().getEulerYPR(fRotZ, fRotY, fRotX);
        core::vector3df IrrRot(fRotX / M_PI * 180, fRotY / M_PI * 180, fRotZ / M_PI * 180);
        m_pSceneNode->setRotation(IrrRot);
    }
#endif // OF_CLIENT
    if(!m_pColObj || m_pColObj->isStaticObject())
        m_bTransformChanged = false;
}

void CObject::AddToWorld()
{
    assert(m_pColObj && m_pColObj->getUserPointer() == this);
    
    if(!m_bIsInWorld)
        m_pLevel->GetColWorld()->AddColObject(this, m_iColGroup, m_iColMask);
    m_bIsInWorld = true;
    
#ifdef OF_CLIENT
    if(m_pSceneNode)
        m_pSceneNode->setVisible(true);
#endif // OF_CLIENT
}

void CObject::RemoveFromWorld()
{
    if(!m_pColObj)
        return;
    
    assert(m_pColObj->getUserPointer() == this);
    
    if(m_bIsInWorld)
        m_pLevel->GetColWorld()->RemoveColObject(this);
    m_bIsInWorld = false;
    
#ifdef OF_CLIENT
    if(m_pSceneNode)
        m_pSceneNode->setVisible(false);
#endif // OF_CLIENT
}
