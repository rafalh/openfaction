/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CVirtualFileSystem.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CVIRTUALFILESYSTEM_H
#define CVIRTUALFILESYSTEM_H

#include "vpp_format.h"
#include "CInputBinaryStream.h"
#include <vector>
#include <string>
#include <map>

class CVirtualFileSystem
{
    public:
        void AddArchive(const char *lpPath);
        void AddArchivesDirectory(const char *pPath);
        std::vector<std::string> FindFiles(const char *pStr = NULL, const char *pExt = NULL) const;
        
        static inline CVirtualFileSystem &GetInst()
        {
            static CVirtualFileSystem Vfs;
            return Vfs;
        }
    
    private:
        std::map<std::string, std::string> m_FileNameToArchive;
        
        void OpenFile(const char *pFileName, FILE *&pFile, std::streamsize &cbFileSize);
    
    friend class CVfsFileBuf;
};

class CVfsFileBuf: public std::streambuf
{
    public:
        inline CVfsFileBuf():
            m_pFile(NULL), m_nPos(0), m_nSize(0) {}
        
        inline ~CVfsFileBuf()
        {
            Close();
        }
        
        inline int Open(const char *pFileName)
        {
            Close();
            
            CVirtualFileSystem::GetInst().OpenFile(pFileName, m_pFile, m_nSize);
        }
        
        inline bool IsOpen() const
        {
            return m_pFile ? true : false;
        }
        
        inline void Close()
        {
            if(m_pFile)
            {
                fclose(m_pFile);
                m_pFile = NULL;
            }
            m_nPos = 0;
            m_nSize = 0;
            setg(NULL, NULL, NULL);
        }
        
        inline unsigned GetSize() const
        {
            return (unsigned)m_nSize;
        }
        
    protected:
        std::streampos seekpos(std::streampos nPos, std::ios_base::openmode Which = std::ios_base::in);
        std::streampos seekoff(std::streamoff nOffset, std::ios_base::seekdir Way, std::ios_base::openmode Which = std::ios_base::in);
        int underflow();
    
    private:
        FILE *m_pFile;
        char m_Buf[64];
        std::streampos m_nPos;
        std::streamsize m_nSize;
};

class CVfsFileStream: private CVfsFileBuf, public CInputBinaryStream
{
    public:
        inline CVfsFileStream():
            CInputBinaryStream(this) {}
        
        inline CVfsFileStream(const char *pFileName):
            CInputBinaryStream(this)
        {
            CVfsFileBuf::Open(pFileName);
        }
        
        inline int Open(const char *pFileName)
        {
            int iStatus = CVfsFileBuf::Open(pFileName);
            clear(iStatus >= 0 ? goodbit : failbit);
            return iStatus;
        }
        
        inline void Close()
        {
            if(CVfsFileBuf::IsOpen())
            {
                CVfsFileBuf::Close();
                clear();
            }
            else
                clear(failbit);
        }
        
        inline bool IsOpen() const
        {
            return CVfsFileBuf::IsOpen();
        }
        
        inline unsigned GetSize() const
        {
            return CVfsFileBuf::GetSize();
        }
};

#endif // CVIRTUALFILESYSTEM_H
