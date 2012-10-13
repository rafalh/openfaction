/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/CString.h
*  PURPOSE:     Improved string class
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSTRING_H_INCLUDED
#define CSTRING_H_INCLUDED

#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

class CString: public std::string
{
    public:
        CString() {}
        CString(const char *psz):
            std::string(psz) {}
        CString(const char *psz, int n):
            std::string(psz, n) {}
        CString(const std::string &str):
            std::string(str) {}
        CString(const std::string &str, int pos, int n = CString::npos):
            std::string(str, pos, n) {}
        
        operator const char*() const
        {
            return c_str();
        }
        
        CString operator+(const char *psz) const
        {
            CString strCopy = *this;
            strCopy.append(psz);
            return strCopy;
        }
        
        CString &operator+=(const char *psz)
        {
            append(psz);
            return *this;
        }
        
        CString operator+(const CString &str) const
        {
            CString strCopy = *this;
            strCopy.append((const std::string)str);
            return strCopy;
        }
        
        CString &operator+=(const CString &str)
        {
            append((const std::string)str);
            return *this;
        }
        
        CString operator+(char ch) const
        {
            CString strCopy = *this;
            strCopy.append(1, ch);
            return strCopy;
        }
        
        CString &operator+=(char ch)
        {
            append(1, ch);
            return *this;
        }
        
        CString operator+(int i) const
        {
            char szBuf[16];
            CString strCopy = *this;
            strCopy.append(itoa(i, szBuf, 10));
            return strCopy;
        }
        
        CString &operator+=(int i)
        {
            char szBuf[16];
            append(itoa(i, szBuf, 10));
            return *this;
        }
        
        char operator[](int i) const
        {
            return at(i);
        }
        
        char &operator[](int i)
        {
            return at(i);
        }
        
        void Format(const char *pszFmt, ...)
        {
            va_list ap;
            va_start(ap, pszFmt); 
            char szBuf[256];
            std::vsnprintf(szBuf, sizeof(szBuf), pszFmt, ap);
            va_end(ap);
            assign(szBuf);
        }
        
        int comparei(int pos, int n, const char *psz)
        {
#if !LINUX
            return _strnicmp(c_str() + pos, psz, n);
#else
            return strncasecmp(c_str() + pos, psz, n);
#endif // !LINUX
        }
        
        int comparei(int pos, int n, const CString &str) const
        {
            return comparei(pos, n, str.c_str());
        }
        
        int comparei(const char *psz, int pos = 0) const
        {
#if !LINUX
            return _stricmp(c_str() + pos, psz);
#else
            return strcasecmp(c_str() + pos, psz);
#endif // !LINUX
        }
        
        int comparei(const CString &str, int pos = 0) const
        {
            return comparei(str.c_str(), pos);
        }
        
        CString lower() const
        {
            CString strTemp;
            strTemp.reserve(size());
            for(int i = 0; i < size(); ++i)
                strTemp += (char)tolower(at(i));
            return strTemp;
        }
};

#endif // CSTRING_H_INCLUDED
