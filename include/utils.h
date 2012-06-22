/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/utils.h
*  PURPOSE:     Some utility functions
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <cstring>
#include <cstdio>
#include <cctype>

#define COUNTOF(array) (sizeof(array)/sizeof(array[0]))

inline int StrCmpI(const char *pStr1, const char *pStr2)
{
#if !LINUX
    return _stricmp(pStr1, pStr2);
#else
    return strcasecmp(pStr1, pStr2);
#endif // !LINUX
}

inline const char *StrStrI(const char *pStr1, const char *pStr2)
{
    unsigned i;
    
    while(*pStr1)
    {
        for(i = 0; pStr1[i] && pStr2[i] && tolower(pStr1[i]) == tolower(pStr2[i]); ++i);
        
        if(!pStr2[i])
            return pStr1;
        
        ++pStr1;
    }
    
    return NULL;
}

#endif // UTILS_H_INCLUDED
