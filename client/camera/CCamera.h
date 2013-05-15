/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/camera/CCamera.h
*  PURPOSE:     Base class for each camera type
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCAMERA_H
#define CCAMERA_H

#include <irrlicht.h>
#include <btBulletDynamicsCommon.h>

class CElement;

class CCamera
{
    public:
        CCamera(irr::scene::ISceneManager *pSceneMgr)
        {
            m_pSceneNode = pSceneMgr->addCameraSceneNode();
        }
        
        virtual ~CCamera()
        {
            m_pSceneNode->remove();
        }
        
        virtual void Update(unsigned uDeltaTime) {}
        virtual bool OnEvent(const irr::SEvent &Event) { return false; }
        virtual void OnActivate() {}
        virtual CElement *GetTarget() const { return 0; }
        
        virtual irr::scene::ICameraSceneNode *GetSceneNode()
        {
            return m_pSceneNode;
        }
        
        btVector3 GetPos() const
        {
            irr::core::vector3df vPos = m_pSceneNode->getPosition();
            return btVector3(vPos.X, vPos.Y, vPos.Z);
        }
    
    protected:
        irr::scene::ICameraSceneNode *m_pSceneNode;
};

#endif // CCAMERA_H
