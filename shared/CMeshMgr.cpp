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
#include "util/CException.h"
#include "util/utils.h"

using namespace std;

CMeshMgr::~CMeshMgr()
{
    ASSERT(m_Meshes.size() == 0);
}

CString CMeshMgr::FixFilename(const CString &strFilename)
{
    if(CVirtualFileSystem::GetInst().DoesFileExists(strFilename))
        return strFilename;
    
    size_t ExtPos = strFilename.rfind('.');
    if(ExtPos == CString::npos)
        return strFilename;
    
    // Try other supported extension
    const char *ExtList[] = {".v3m", ".v3c"};
    for(unsigned i = 0; i < COUNTOF(ExtList); ++i)
    {
        CString strNewFilename = strFilename.substr(0, ExtPos);
        strNewFilename += ExtList[i];
        
        if(CVirtualFileSystem::GetInst().DoesFileExists(strNewFilename))
            return strNewFilename;
    }
    
    // Failed
    return strFilename;
}

CMesh *CMeshMgr::Load(const CString &strFilename)
{
    map<CString, CMesh*>::iterator it = m_Meshes.find(strFilename);
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
        CString strNewFilename = FixFilename(strFilename);
        CVfsFileStream File(strNewFilename);
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
    map<CString, CMesh*>::iterator it;
    for(it = m_Meshes.begin(); it != m_Meshes.end(); ++it)
        if(it->second == pMesh)
        {
            m_Meshes.erase(it);
            return;
        }
    
    assert(false);
}
