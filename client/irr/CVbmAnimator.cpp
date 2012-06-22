/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CVbmAnimator.cpp
*  PURPOSE:     Texture animator for Irrlicht
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "irr/CVbmAnimator.h"
#include "CMaterial.h"

using namespace std;
using namespace irr;

CIrrVbmAnimator::CIrrVbmAnimator(CMaterial *pMaterial, unsigned iMaterial):
    m_pMaterial(pMaterial), m_iMaterial(iMaterial)
{
    m_pMaterial->AddRef();
}

CIrrVbmAnimator::~CIrrVbmAnimator()
{
    m_pMaterial->Release();
}

void CIrrVbmAnimator::animateNode(scene::ISceneNode *pNode, u32 uTimeMs)
{
    if(!pNode || !m_pMaterial || !m_pMaterial->GetFramesCount())
		return;
    
    unsigned uAnimationTime = uTimeMs % (m_pMaterial->GetFramesCount() * 1000 / m_pMaterial->GetFps());
    unsigned iFrame = uAnimationTime * m_pMaterial->GetFps() / 1000;
    
    pNode->getMaterial(m_iMaterial).setTexture(0, m_pMaterial->GetFrame(iFrame));
}

scene::ISceneNodeAnimator *CIrrVbmAnimator::createClone(scene::ISceneNode *pNode, scene::ISceneManager *pNewManager)
{
	return new CIrrVbmAnimator(m_pMaterial, m_iMaterial);
}
