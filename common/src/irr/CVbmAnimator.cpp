/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CVbmAnimator.cpp
*  PURPOSE:     Texture animator for Irrlicht
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#define OF_CLIENT // FIXME

#include "irr/CVbmAnimator.h"
#include "CMultiTexture.h"

using namespace std;
using namespace irr;

CIrrVbmAnimator::CIrrVbmAnimator(CMultiTexture *pMaterial, unsigned iMaterial):
    m_pMultiTexture(pMaterial), m_iMaterial(iMaterial)
{
    m_pMultiTexture->AddRef();
}

CIrrVbmAnimator::~CIrrVbmAnimator()
{
    m_pMultiTexture->Release();
}

void CIrrVbmAnimator::animateNode(scene::ISceneNode *pNode, u32 uTimeMs)
{
    if(!pNode || !m_pMultiTexture || !m_pMultiTexture->GetFramesCount())
		return;
    
    unsigned uAnimationTime = uTimeMs % (m_pMultiTexture->GetFramesCount() * 1000 / m_pMultiTexture->GetFps());
    unsigned uFrame = uAnimationTime * m_pMultiTexture->GetFps() / 1000;
    
    pNode->getMaterial(m_iMaterial).setTexture(0, m_pMultiTexture->GetFrame(uFrame));
}

scene::ISceneNodeAnimator *CIrrVbmAnimator::createClone(scene::ISceneNode *pNode, scene::ISceneManager *pNewManager)
{
	return new CIrrVbmAnimator(m_pMultiTexture, m_iMaterial);
}
