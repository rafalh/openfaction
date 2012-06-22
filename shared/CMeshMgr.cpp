/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMeshMgr.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CMeshMgr.h"
#include "CMesh.h"
#include "CVirtualFileSystem.h"
#include "CConsole.h"
#include "CGame.h"

using namespace std;

CMeshMgr::~CMeshMgr()
{
    assert(m_Meshes.size() == 0);
}

CMesh *CMeshMgr::Load(const char *pszFilename)
{
    map<string, CMesh*>::iterator it = m_Meshes.find(pszFilename);
    if(it != m_Meshes.end())
    {
        it->second->AddRef();
        return it->second;
    }
    
    CMesh *pMesh = new CMesh(this);
    m_Meshes[pszFilename] = pMesh;
    
    //m_pGame->GetConsole()->DbgPrint("Loading mesh: %s\n", pszFilename);
    CVfsFileStream File(pszFilename);
    if(pMesh->Load(File) < 0)
    {
        m_pGame->GetConsole()->DbgPrint("Failed to load mesh: %s\n", pszFilename);
        pMesh->Release();
        return NULL;
    }
    
    return pMesh;
}

void CMeshMgr::Remove(CMesh *pMesh)
{
    map<string, CMesh*>::iterator it;
    for(it = m_Meshes.begin(); it != m_Meshes.end(); ++it)
        if(it->second == pMesh)
        {
            m_Meshes.erase(it);
            return;
        }
    assert(false);
}
