/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CServerConsole.h
*  PURPOSE:     Server console implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSERVERCONSOLE_H
#define CSERVERCONSOLE_H

#include "util/CString.h"
#include <cstdarg>
#include <cstdio>
#include "CConsole.h"

class CPlayer;

class CServerConsole: public CConsole
{
    public:
        void Run();
        void CmdOutput(TUser hUser, const char *pszFormat, ...);
        
        void VPrint(const char *pFormat, va_list pArgList)
        {
            vprintf(pFormat, pArgList);
        }
    
    private:
        static const char *m_pCmdPrompt;
};

#endif // CSERVERCONSOLE_H
