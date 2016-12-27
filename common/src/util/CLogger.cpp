/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CLogger.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "util/CLogger.h"
#include "CConsole.h"
#include <ctime>

using namespace std;

void CLogger::PrintStatus(const char *pFormat, ...)
{
    PrintTime(false);
    
    va_list pArgList;
    va_start(pArgList, pFormat);
    if(m_pConsole)
        m_pConsole->VPrint(pFormat, pArgList);
    va_end(pArgList);
    
    if(m_pConsole)
        m_pConsole->Print("\n");
    
    if(m_pFile)
    {
        va_start(pArgList, pFormat);
        vfprintf(m_pFile, pFormat, pArgList);
        fprintf(m_pFile, "\n");
        fflush(m_pFile);
    }
}
        
void CLogger::PrintError(const char *pFormat, ...)
{
    PrintTime(true);
    
    va_list pArgList;
    va_start(pArgList, pFormat);
    if(m_pConsole)
        m_pConsole->VPrint(pFormat, pArgList);
    va_end(pArgList);
    
    if(m_pConsole)
        m_pConsole->Print("\n");
    
    if(m_pFile)
    {
        va_start(pArgList, pFormat);
        vfprintf(m_pFile, pFormat, pArgList);
        fprintf(m_pFile, "\n");
        fflush(m_pFile);
    }
}

void CLogger::PrintTime(bool bError)
{
    time_t Timestamp = time(NULL);
    const struct tm *pTm = localtime(&Timestamp);
    
    if(m_pConsole)
        m_pConsole->Print("[%02u:%02u:%02u] ", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
    
    if(m_pFile)
        fprintf(m_pFile, "[%02u:%02u:%02u] ", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
}
