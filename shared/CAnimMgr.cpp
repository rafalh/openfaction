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
#include "CException.h"
#include "CGame.h"
#include "CConsole.h"

using namespace std;

CAnimMgr::~CAnimMgr()
{
    assert(m_Animations.empty());
}

string CAnimMgr::FixFilename(const string &strFilename)
{
    size_t ExtPos = strFilename.rfind('.');
    if(ExtPos != string::npos)
        return strFilename.substr(0, ExtPos) + ".rfa";
    
    return strFilename;
}

CAnimation *CAnimMgr::Load(const string &strFilename)
{
    map<string, CAnimation*>::iterator it = m_Animations.find(strFilename);
    if(it != m_Animations.end())
    {
        CAnimation *pAnim = it->second;
        pAnim->AddRef();
        return pAnim;
    }
    
    CAnimation *pAnim = new CAnimation(this);
    m_Animations.insert(pair<string, CAnimation*>(strFilename, pAnim));
    
    //m_pGame->GetConsole()->DbgPrint("Loading animation %s\n", strFilename.c_str());
    
    try
    {
        string strNewFilename = FixFilename(strFilename);
        CVfsFileStream File(strNewFilename.c_str());
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
    map<string, CAnimation*>::iterator it;
    for(it = m_Animations.begin(); it != m_Animations.end(); ++it)
        if(it->second == pAnim)
        {
            m_Animations.erase(it);
            return;
        }
    
    assert(false);
}
