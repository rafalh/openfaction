/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMaterialsMgr.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CTextureMgr.h"

CTextureMgr::~CTextureMgr()
{
    assert(m_Materials.empty());
}

CMultiTexture *CTextureMgr::Load(const CString &strFilename)
{
    auto it = m_Materials.find(strFilename);
    if(it != m_Materials.end())
    {
        it->second->AddRef();
        return it->second;
    }
    
    CMultiTexture *pMaterial = new CMultiTexture(this, strFilename);
    m_Materials.insert(std::make_pair(strFilename, pMaterial));
    return pMaterial;
}

void CTextureMgr::Remove(CMultiTexture *pMaterial)
{
    std::map<CString, CMultiTexture*>::iterator it;
    for(it = m_Materials.begin(); it != m_Materials.end(); ++it)
        if(it->second == pMaterial)
        {
            m_Materials.erase(it);
            return;
        }
    
    assert(false);
}
