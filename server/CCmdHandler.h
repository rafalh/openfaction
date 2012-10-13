/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CCmdHandler.h
*  PURPOSE:     Command handlers for server console
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCMDHANDLER_H
#define CCMDHANDLER_H

#include "CString.h"
#include "CConsole.h"

class CServer;

class CCmdHandler: public CConsole::CCmdHandler
{
    public:
        CCmdHandler(CConsole *pConsole, CServer *pServer):
            m_pConsole(pConsole), m_pServer(pServer) {}
        bool HandleCmd(const char *pCommand, const char *pArg, CConsole::TUser hUser);
    
    private:
        CConsole *m_pConsole;
        CServer *m_pServer;
        CString m_strLastBan;
};

#endif // CCMDHANDLER_H
