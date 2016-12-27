/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CAnimMgr.cpp
*  PURPOSE:     Animations manager
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CAnimMgr.h"
#include "CVirtualFileSystem.h"
#include "util/CException.h"
#include "util/CLogger.h"
#include "util/utils.h"

using namespace std;

CAnimMgr::~CAnimMgr()
{
    assert(m_Animations.empty());
}

CString CAnimMgr::FixFilename(const CString &strFilename)
{
    size_t ExtPos = strFilename.rfind('.');
    if(ExtPos != CString::npos)
        return strFilename.substr(0, ExtPos) + ".rfa";
    
    return strFilename;
}

CAnimation *CAnimMgr::Load(const CString &strFilename)
{
    map<CString, CAnimation*>::iterator it = m_Animations.find(strFilename);
    if(it != m_Animations.end())
    {
        CAnimation *pAnim = it->second;
        pAnim->AddRef();
        return pAnim;
    }
    
    CAnimation *pAnim = new CAnimation(this);
    m_Animations.insert(pair<CString, CAnimation*>(strFilename, pAnim));
    
    //CLogger::GetInst().PrintStatus("Loading animation %s\n", strFilename.c_str());
    
    try
    {
        CString strNewFilename = FixFilename(strFilename);
        CVfsFileStream File(strNewFilename);
        pAnim->Load(File);
    }
    catch(const exception &e)
    {
        pAnim->Release();
        THROW_EXCEPTION("Failed to load %s:\n%s", strFilename.c_str(), e.what());
    }
    
    return pAnim;
}

void CAnimMgr::Remove(CAnimation *pAnim)
{
    map<CString, CAnimation*>::iterator it;
    for(it = m_Animations.begin(); it != m_Animations.end(); ++it)
        if(it->second == pAnim)
        {
            m_Animations.erase(it);
            return;
        }
    
    ASSERT(false);
}
