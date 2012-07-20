/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMeshMgr.cpp
*  PURPOSE:     Mesh manager
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

string CMeshMgr::FixFilename(const string &strFilename)
{
    if(CVirtualFileSystem::GetInst().DoesFileExists(strFilename.c_str()))
        return strFilename;
    
    size_t ExtPos = strFilename.rfind('.');
    if(ExtPos == string::npos)
        return strFilename;
    
    // Try other supported extension
    const char *ExtList[] = {".v3m", ".v3c"};
    for(unsigned i = 0; i < COUNTOF(ExtList); ++i)
    {
        string strNewFilename = strFilename.substr(0, ExtPos);
        strNewFilename += ExtList[i];
        
        if(CVirtualFileSystem::GetInst().DoesFileExists(strNewFilename.c_str()))
            return strNewFilename;
    }
    
    // Failed
    return strFilename;
}

CMesh *CMeshMgr::Load(const string &strFilename)
{
    map<string, CMesh*>::iterator it = m_Meshes.find(strFilename);
    if(it != m_Meshes.end())
    {
        it->second->AddRef();
        return it->second;
    }
    
    CMesh *pMesh = new CMesh(this);
    m_Meshes[strFilename] = pMesh;
    
    //m_pGame->GetConsole()->DbgPrint("Loading mesh: %s\n", strFilename.c_str());
    
    try
    {
        string strNewFilename = FixFilename(strFilename);
        CVfsFileStream File(strNewFilename.c_str());
        pMesh->Load(File);
    }
    catch(const exception &e)
    {
        pMesh->Release();
        THROW_EXCEPTION("Failed to load %s:\n%s", strFilename.c_str(), e.what());
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
