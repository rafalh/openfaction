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
#include "CException.h"
#include "utils.h"

using namespace std;

CMeshMgr::~CMeshMgr()
{
    assert(m_Meshes.size() == 0);
}

std::string CMeshMgr::FixFilename(const char *pszFilename)
{
    if(CVirtualFileSystem::GetInst().DoesFileExists(pszFilename))
        return pszFilename;
    
    // Try other supported extension
    const char *ExtList[] = {".v3m", ".v3c"};
    string strFilename = pszFilename;
    
    for(unsigned i = 0; i < COUNTOF(ExtList); ++i)
    {
        size_t ExtPos = strFilename.rfind('.');
        if(ExtPos != string::npos)
            strFilename.erase(ExtPos);
        strFilename += ExtList[i];
        
        if(CVirtualFileSystem::GetInst().DoesFileExists(strFilename.c_str()))
            return strFilename;
    }
    
    // Failed
    return pszFilename;
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
    
    try
    {
        string strFilename = FixFilename(pszFilename);
        CVfsFileStream File(strFilename.c_str());
        pMesh->Load(File);
    }
    catch(const exception &e)
    {
        pMesh->Release();
        THROW_EXCEPTION("Failed to load %s:\n%s", pszFilename, e.what());
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
