/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CLodSceneNodeAnimator.h
*  PURPOSE:     Level of detail mesh scene node animator for Irrlicht
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#pragma once

#include <irrlicht.h>
#include <map>

class CLodSceneNodeAnimator : public irr::scene::ISceneNodeAnimator
{
public:
    CLodSceneNodeAnimator() {}
    virtual ~CLodSceneNodeAnimator();
    void animateNode(irr::scene::ISceneNode* node, irr::u32 uTimeMs);
    ISceneNodeAnimator *createClone(irr::scene::ISceneNode *node, irr::scene::ISceneManager *newManager);

    void addLevelOfDetail(irr::scene::IMesh *mesh, float minDistance)
    {
        mesh->grab();
        m_LodMeshMap.insert(std::make_pair(minDistance, mesh));
    }

private:
    std::map<float, irr::scene::IMesh*> m_LodMeshMap;
};
