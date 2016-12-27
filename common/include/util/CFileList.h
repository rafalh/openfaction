/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/CFileList.h
*  PURPOSE:     Utility for finding files in local filesystem
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFILELIST_H
#define CFILELIST_H

#if !LINUX

#include <windows.h>

class CFileList
{
    public:
        enum
        {
            FILE = 0,
            DIR = 1,
        };
        
        inline CFileList(const char *pPattern)
        {
            m_hFind = FindFirstFileA(pPattern, &m_Wfd);
        }

        inline ~CFileList()
        {
            if(m_hFind != INVALID_HANDLE_VALUE)
                FindClose(m_hFind);
        }

        /* Returns 1 for directory, 0 for file */
        inline int GetNextFile(std::string &strName)
        {
            int iRet;

            do
            {
                if(m_hFind == INVALID_HANDLE_VALUE)
                    return -1;

                strName = m_Wfd.cFileName;
                iRet = (m_Wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DIR : FILE;

                if(!FindNextFileA(m_hFind, &m_Wfd))
                {
                    FindClose(m_hFind);
                    m_hFind = INVALID_HANDLE_VALUE;
                }
            } while(strName == "." || strName == "..");

            return iRet;
        }

    private:
        HANDLE m_hFind;
        WIN32_FIND_DATAA m_Wfd;
};

#else // LINUX
#include <glob.h>

class CFileList
{
    public:
        inline CFileList(const char *pPattern)
        {
            glob(pPattern, GLOB_MARK, NULL, &m_GlobBuf);
            m_nCurrentPath = 0;
        }

        inline ~CFileList()
        {
            globfree(&m_GlobBuf);
        }

        /* Returns 1 for directory, 0 for file */
        inline int GetNextFile(std::string &strName)
        {
            int iRet;

            do
            {
                if(m_nCurrentPath >= m_GlobBuf.gl_pathc)
                    return -1;

                const char *pszFilename = m_GlobBuf.gl_pathv[m_nCurrentPath];
                if(pszFilename[0] == '/')
                {
                    strName = pszFilename + 1;
                    iRet = DIR;
                }
                else
                {
                    strName = pszFilename;
                    iRet = FILE;
                }
            } while(strName == "." || strName == "..");

            return iRet;
        }

    private:
        unsigned m_nCurrentPath;
        glob_t m_GlobBuf;
};

#endif // LINUX

#endif // CFILELIST_H
