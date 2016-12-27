/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CStringsTable.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "util/utils.h"
#include "tables/CStringsTable.h"
#include "tables/CTblReader.h"
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <algorithm>

using namespace std;

int CStringsTable::Load(istream &Stream)
{
    CTblReader Reader(Stream, "Strings");
    if(!Reader.IsSectionFound())
        return -1;
    
    unsigned nStrId = 0;
    while(Reader.LoadNextElement() == 0)
    {
        const char *pName = Reader.GetName();
        if(pName[0] == '#')
            nStrId = strtoul(pName + 1, NULL, 0);
        else if(!StrCmpI(pName, "En"))
            Reader.GetString(m_Strings[nStrId]);
    }
    
    return 0;
}

const char *CStringsTable::GetFormated(unsigned nId, ...)
{
    std::map<unsigned, CString>::const_iterator it = m_Strings.find(nId);
    
    if(it == m_Strings.end())
        return "";
    
    va_list pArgList;
    va_start(pArgList, nId);
    
    vsnprintf(m_Buffer, sizeof(m_Buffer), it->second, pArgList);
    
    va_end(pArgList);
    
    return m_Buffer;
}

const char *CStringsTable::ParseXStr(const char *pXStr)
{
    assert(strncmp(pXStr, "XSTR(", 5) == 0);
    
    const char *Ptr = NULL;
    unsigned nId = strtoul(pXStr + 5, (char**)&Ptr, 0);
    std::map<unsigned, CString>::const_iterator it = m_Strings.find(nId);
    if(it != m_Strings.end())
        return it->second;
    
    if(!Ptr || Ptr[0] != ',')
        return "";
    
    for(++Ptr; isspace(*Ptr); ++Ptr);
    
    if(Ptr[0] != '"')
        return "";
    ++Ptr;
    
    const char *pStrEnd = strchr(Ptr, '"');
    if(!pStrEnd)
        return "";
    
    unsigned cchBuf = min(pStrEnd - Ptr, (int)sizeof(m_Buffer) - 1);
    memcpy(m_Buffer, Ptr, cchBuf);
    m_Buffer[cchBuf] = '\0';
    return m_Buffer;
}
