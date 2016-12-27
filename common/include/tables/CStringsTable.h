/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CStringsTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSTRINGSTABLE_H
#define CSTRINGSTABLE_H

#include <map>
#include "util/CString.h"

class CStringsTable
{
    public:
        int Load(std::istream &Stream);
        
        inline const char *Get(unsigned nId) const
        {
            std::map<unsigned, CString>::const_iterator it = m_Strings.find(nId);
            
            if(it == m_Strings.end())
                return "";
            
            return it->second;
        }
        
        const char *GetFormated(unsigned nId, ...);
        const char *ParseXStr(const char *pXStr);
    
    private:
        char m_Buffer[256];
        std::map<unsigned, CString> m_Strings;
};

#endif // CSTRINGSTABLE_H
