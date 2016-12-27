/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CLogger.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CLOGGER_H
#define CLOGGER_H

#include <cstdio>

class CConsole;

class CLogger
{
    public:
        CLogger():
            m_pFile(NULL), m_pConsole(NULL) {}
        
        ~CLogger()
        {
            Cleanup();
        }
        
        void PrintStatus(const char *pFormat, ...);
        void PrintError(const char *pFormat, ...);
        
        bool IsEnabled() const
        {
            return m_pFile ? true : false;
        }
        
        int SetPath(const char *pPath)
        {
            if(m_pFile)
                fclose(m_pFile);
            
            if(pPath)
            {
                m_pFile = fopen(pPath, "a");
                if(!m_pFile)
                    return -1;
            }
            else
                m_pFile = NULL;
            
            return 0;
        }
        
        void SetConsole(CConsole *pConsole)
        {
            m_pConsole = pConsole;
        }
        
        CConsole *GetConsole() const
        {
            return m_pConsole;
        }
        
        void Cleanup()
        {
            if(m_pFile)
                fclose(m_pFile);
            m_pFile = NULL;
        }
        
        static CLogger &GetInst()
        {
            static CLogger Logger;
            return Logger;
        }
    
    private:
        void PrintTime(bool bError);
        
        FILE *m_pFile;
        CConsole *m_pConsole;
};

#endif // CLOGGER_H
