/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CAnimMgr.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CAnimMgr.h"

using namespace std;

CAnimMgr::~CAnimMgr()
{
    assert(m_Animations.empty());
}

CAnimation *CAnimMgr::Load(std::string strFilename)
{
    map<string, CAnimation*>::iterator it = m_Animations.find(strFilename);
    if(it != m_Animations.end())
    {
        it->second->AddRef();
        return it->second;
    }
    
    CAnimation *pAnim = new CAnimation(this);
    m_Animations.insert(pair<string, CAnimation*>(strFilename, pAnim));
    return pAnim;
}

void CAnimMgr::Remove(CAnimation *pAnim)
{
    map<string, CAnimation*>::iterator it;
    for(it = m_Animations.begin(); it != m_Animations.end(); ++it)
        if(it->second == pAnim)
        {
            m_Animations.erase(it);
            break;
        }
}
