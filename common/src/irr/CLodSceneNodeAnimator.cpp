/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CLodSceneNodeAnimator.cpp
*  PURPOSE:     Level of detail mesh scene node animator for Irrlicht
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "irr/CLodSceneNodeAnimator.h"
#include "util/CLogger.h"

using namespace std;
using namespace irr;

CLodSceneNodeAnimator::~CLodSceneNodeAnimator()
{
    map<float, scene::IMesh*>::const_iterator it = m_LodMeshMap.begin();
    while (it != m_LodMeshMap.end())
    {
        it->second->drop();
        ++it;
    }
    m_LodMeshMap.clear();
}

void CLodSceneNodeAnimator::animateNode(scene::ISceneNode *node, u32 timeMs)
{
    if (!node)
        return;

    const core::vector3df &CameraPos = node->getSceneManager()->getActiveCamera()->getPosition();
    const core::vector3df &MeshNodePos = node->getPosition();
    float fDist = CameraPos.getDistanceFrom(MeshNodePos);

    scene::IMeshSceneNode *pMeshNode = dynamic_cast<scene::IMeshSceneNode*>(node);
    if (!pMeshNode)
        return;

    scene::IMesh *pLodMesh = nullptr;
    map<float, scene::IMesh*>::const_reverse_iterator it = m_LodMeshMap.rbegin();
    while (it != m_LodMeshMap.rend())
    {
        if (fDist >= it->first)
        {
            pLodMesh = it->second;
            break;
        }
        ++it;
    }

    if (pMeshNode->getMesh() != pLodMesh)
    {
        //CLogger::GetInst().PrintStatus("Switching LOD %p\n", pMeshNode);
        pMeshNode->setMesh(pLodMesh);
    }
}

scene::ISceneNodeAnimator *CLodSceneNodeAnimator::createClone(scene::ISceneNode *node, scene::ISceneManager *newManager)
{
    CLodSceneNodeAnimator *pClone = new CLodSceneNodeAnimator();
    map<float, scene::IMesh*>::const_iterator it = m_LodMeshMap.begin();
    while (it != m_LodMeshMap.end())
    {
        pClone->addLevelOfDetail(it->second, it->first);
        ++it;
    }
    return pClone;
}
