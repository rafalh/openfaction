/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CFoleyTable.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CFoleyTable.h"
#include "CTblReader.h"
#include "utils.h"

using namespace std;

int CFoleyTable::Load(istream &Stream)
{
    CTblReader Reader(Stream, "Entity Sounds");
    if(!Reader.IsSectionFound())
        return -1;
    
    SFoleySound *pCurrent = NULL;
    
    while(Reader.LoadNextElement() == 0)
    {
        const char *pszName = Reader.GetName();
        
        if(!StrCmpI(pszName, "$Name"))
        {
            SFoleySound Snd;
            Reader.GetString(Snd.strName);
            
            pCurrent = &m_Sounds.insert(pair<CString, SFoleySound>(Snd.strName, Snd)).first->second;
        }
        else if(pCurrent)
        {
            if(!StrCmpI(pszName, "$Material"))
                Reader.GetString(pCurrent->strMaterial);
            else if(!StrCmpI(pszName, "$Sound"))
            {
                SFoleySound::SSound Snd;
                Reader.GetString(Snd.strFilename);
                Reader.GetFloat(Snd.fMinDist);
                Reader.GetFloat(Snd.fVolume);
                pCurrent->Sounds.push_back(Snd);
            }
        }
    }
    
    return 0;
}

const SFoleySound *CFoleyTable::Get(const CString &strName) const
{
    map<CString, SFoleySound>::const_iterator it = m_Sounds.find(strName);
    if(it == m_Sounds.end())
        return NULL;
    
    return &it->second;
}
