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
                iRet = (m_Wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;

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

#else /* LINUX */
#include <glob.h>

class CFileList
{
    public:
        inline CFileList(const char *pPattern)
        {
            m_GlobBuf.gl_offs = 0;
            glob(pPattern, GLOB_DOOFFS, NULL, &m_GlobBuf);
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

                strName = m_GlobBuf.gl_pathv[m_nCurrentPath]; // FIXME: is it a full path or what?
                iRet = 0; // FIXME: check if it is a dir
            } while(strName == "." || strName == "..");

            return iRet;
        }

    private:
        unsigned m_nCurrentPath;
        glob_t m_GlobBuf;
};

#endif

#endif // CFILELIST_H
