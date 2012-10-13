/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CMpCharactersTable.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CMpCharactersTable.h"
#include "CTblReader.h"
#include "utils.h"
#include "CGame.h"

using namespace std;

SMpCharacter::SMpCharacter(unsigned _nId)
{
    nId = _nId;
    pEntityClass = NULL;
    pEntityAnimClass = NULL;
}

int CMpCharactersTable::Load(istream &Stream)
{
    CTblReader Reader(Stream, "Characters");
    if(!Reader.IsSectionFound())
        return -1;
    
    while(Reader.LoadNextElement() == 0)
    {
        const char *pName = Reader.GetName();
        
        if(!StrCmpI(pName, "$Name"))
        {
            SMpCharacter Character(m_MpCharacters.size());
            Reader.GetString(Character.strName);
            
            m_MpCharacters.push_back(Character);
        }
        else if(!m_MpCharacters.empty())
        {
            if(!StrCmpI(pName, "$EntityType"))
            {
                CString strEntityType;
                Reader.GetString(strEntityType);
                m_MpCharacters.back().pEntityClass = m_pGame->GetEntitiesTbl()->Get(strEntityType);
            }
            else if(!StrCmpI(pName, "$EntityAnimType"))
            {
                CString strEntityAnimType;
                Reader.GetString(strEntityAnimType);
                m_MpCharacters.back().pEntityAnimClass = m_pGame->GetEntitiesTbl()->Get(strEntityAnimType);
            }
        }
    }
    
    return 0;
}
