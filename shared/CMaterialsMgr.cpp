/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMaterialsMgr.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CMaterialsMgr.h"

using namespace std;

CMaterialsMgr::~CMaterialsMgr()
{
    assert(m_Materials.empty());
}

CMaterial *CMaterialsMgr::Load(const string &strFilename)
{
    map<string, CMaterial*>::iterator it = m_Materials.find(strFilename);
    if(it != m_Materials.end())
    {
        it->second->AddRef();
        return it->second;
    }
    
    CMaterial *pMaterial = new CMaterial(this, strFilename);
    m_Materials.insert(pair<string, CMaterial*>(strFilename, pMaterial));
    return pMaterial;
}

void CMaterialsMgr::Remove(CMaterial *pMaterial)
{
    map<string, CMaterial*>::iterator it;
    for(it = m_Materials.begin(); it != m_Materials.end(); ++it)
        if(it->second == pMaterial)
        {
            m_Materials.erase(it);
            return;
        }
    
    assert(false);
}
