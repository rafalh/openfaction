/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CException.h
*  PURPOSE:     Exception handling helpers
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CEXCEPTION_H
#define CEXCEPTION_H

#include <exception>
#include <string>
#include <cstdarg>
#include <cstdio>

#define THROW_EXCEPTION(fmt, ...) throw CException(fmt " in %s:%u", ##__VA_ARGS__, __FILE__, __LINE__)

class CException: public std::exception
{
    public:
        inline CException(const char *pszFormat, ...)
        {
            va_list ap;
            va_start(ap, pszFormat); 
            char szBuf[256];
            std::vsnprintf(szBuf, sizeof(szBuf), pszFormat, ap);
            va_end(ap);
            m_strWhat = szBuf;
        }
        
        inline ~CException() throw() {}
        
        inline const char *what() const throw()
        {
            return m_strWhat.c_str();
        }
    
    private:
        std::string m_strWhat;
};

#endif // CEXCEPTION_H
