/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CMpCharactersTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CMPCHARACTERSTABLE_H
#define CMPCHARACTERSTABLE_H

#include <vector>
#include "CString.h"
#include "CEntitiesTable.h"

class CGame;

struct SMpCharacter
{
    SMpCharacter(unsigned nId);
    
    unsigned nId;
    CString strName;
    const SEntityClass *pEntityClass, *pEntityAnimClass;
};

class CMpCharactersTable
{
    public:
        CMpCharactersTable(CGame *pGame):
            m_pGame(pGame) {}
        
        int Load(std::istream &Stream);
        
        inline const SMpCharacter *Get(unsigned nId) const
        {
            if(nId >= m_MpCharacters.size())
                return NULL;
            
            return &m_MpCharacters[nId];
        }
    
    private:
        CGame *m_pGame;
        std::vector<SMpCharacter> m_MpCharacters;
};

#endif // CMPCHARACTERSTABLE_H
