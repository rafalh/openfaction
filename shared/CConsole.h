/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CConsole.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCONSOLE_H
#define CCONSOLE_H

#include <cstdarg>

class CConsole
{
    public:
        typedef void *TUser;
        
        class CCmdHandler
        {
            public:
                virtual bool HandleCmd(const char *pCommand, const char *pArg, TUser pUser = 0) = 0;
        };
        
        CConsole():
            m_pCmdHandler(NULL) {}
        
        virtual ~CConsole() {}
        virtual void VPrint(const char *pszFormat, std::va_list ap) = 0;
        
        void Print(const char *pszFormat, ...)
        {
            std::va_list ap;
            va_start(ap, pszFormat);
            VPrint(pszFormat, ap);
            va_end(ap);
        }
        
        void DbgPrint(const char *pszFormat, ...)
        {
#ifdef DEBUG
            std::va_list ap;
            va_start(ap, pszFormat);
            VPrint(pszFormat, ap);
            va_end(ap);
#endif
        }
        
        void SetCmdHandler(CCmdHandler *pCmdHandler)
        {
            m_pCmdHandler = pCmdHandler;
        }
        
        virtual void CmdOutput(TUser hUser, const char *pszFormat, ...)
        {
            std::va_list ap;
            va_start(ap, pszFormat);
            VPrint(pszFormat, ap);
            va_end(ap);
        }
        
        virtual bool ExecuteCommand(const char *pszCommand, const char *pszArg, TUser hUser = 0)
        {
            bool bExit = false;
            
            if(m_pCmdHandler)
                bExit = m_pCmdHandler->HandleCmd(pszCommand, pszArg, hUser);
            
            return !bExit;
        }
    
    private:
        CCmdHandler *m_pCmdHandler;
};

#endif // CCONSOLE_H
