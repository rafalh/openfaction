/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CMpCharactersTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#pragma once

#include <vector>
#include "util/CString.h"
#include "tables/CEntitiesTable.h"

class CTablesMgr;

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
        CMpCharactersTable(CTablesMgr *pTablesMgr):
            m_pTablesMgr(pTablesMgr) {}
        
        int Load(std::istream &Stream);
        
        inline const SMpCharacter *Get(unsigned nId) const
        {
            if(nId >= m_MpCharacters.size())
                return NULL;
            
            return &m_MpCharacters[nId];
        }
    
    private:
        CTablesMgr *m_pTablesMgr;
        std::vector<SMpCharacter> m_MpCharacters;
};
