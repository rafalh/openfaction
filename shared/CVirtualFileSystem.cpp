/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CVirtualFileSystem.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CVirtualFileSystem.h"
#include "CFileList.h"
#include "utils.h"
#include "CException.h"
#include "errors.h"

#define ALIGN(offset, alignment) (((offset) + (alignment) - 1) & (~(alignment - 1)))

using namespace std;

void CVirtualFileSystem::AddArchive(const char *pszPath)
{
    FILE *pFile = fopen(pszPath, "rb");
    if(!pFile)
        THROW_EXCEPTION("Failed to open file %s", pszPath);
    
    vpp_header_t Header;
    if(fread(&Header, sizeof(Header), 1, pFile) != 1)
    {
        fclose(pFile);
        THROW_EXCEPTION("Failed to read archive %s header", pszPath);
    }
    else if(Header.signature != VPP_SIGNATURE)
    {
        fclose(pFile);
        THROW_EXCEPTION("Archive %s has invalid signature", pszPath);
    }
    else if(Header.version != 1)
    {
        fclose(pFile);
        THROW_EXCEPTION("Archive %s has unknown version", pszPath);
    }
    else if(fseek(pFile, VPP_ALIGNMENT, SEEK_SET) != 0)
    {
        fclose(pFile);
        THROW_EXCEPTION("Failed to seek");
    }
    
    for(unsigned i = 0; i < Header.file_count; ++i)
    {
        vpp_entry_t FileEntry;
        if(fread(&FileEntry, sizeof(FileEntry), 1, pFile) != 1)
            break;
        
        string strFileName;
        for(const char *Ptr = FileEntry.file_name; *Ptr; ++Ptr)
            strFileName += tolower(*Ptr);
        
        m_FileNameToArchive[strFileName] = pszPath;
    }
    
    fclose(pFile);
}

void CVirtualFileSystem::OpenFile(const char *pFileName, FILE *&pFile, std::streamsize &cbFileSize)
{
    string strFileName;
        for(const char *Ptr = pFileName; *Ptr; ++Ptr)
            strFileName += tolower(*Ptr);
    
    map<string, string>::iterator it = m_FileNameToArchive.find(strFileName);
    if(it == m_FileNameToArchive.end())
        THROW_EXCEPTION("File %s not found", pFileName);
    
    pFile = fopen(it->second.c_str(), "rb");
    if(!pFile)
        THROW_EXCEPTION("Archive %s not found", it->second.c_str());
    
    vpp_header_t Header;
    if(fread(&Header, sizeof(Header), 1, pFile) != 1 ||
       Header.signature != VPP_SIGNATURE ||
       Header.version != 1 ||
       fseek(pFile, VPP_ALIGNMENT, SEEK_SET) != 0)
    {
        fclose(pFile);
        pFile = NULL;
        THROW_EXCEPTION("Invalid archive header");
    }
    
    unsigned nOffset = VPP_ALIGNMENT + ALIGN(Header.file_count * sizeof(vpp_entry_t), VPP_ALIGNMENT);
    
    for(unsigned i = 0; i < Header.file_count; ++i)
    {
        vpp_entry_t FileEntry;
        if(fread(&FileEntry, sizeof(FileEntry), 1, pFile) != 1)
            break;
        
        if(StrCmpI(FileEntry.file_name, pFileName) == 0)
        {
            if(fseek(pFile, nOffset, SEEK_SET) != 0)
                break;
            
            cbFileSize = FileEntry.file_size;
            return;
        }
        
        nOffset += ALIGN(FileEntry.file_size, VPP_ALIGNMENT);
    }
    
    fclose(pFile);
    pFile = NULL;
    THROW_EXCEPTION("File not found");
}

void CVirtualFileSystem::AddArchivesDirectory(const char *pPath)
{
    std::string strPattern = string(pPath) + "*.vpp";
    CFileList FileList(strPattern.c_str());
    while(true)
    {
        string strName;
        int iStatus = FileList.GetNextFile(strName);
        if(iStatus < 0)
            break;
        
        if(iStatus == 0) // file
        {
            string strPath = string(pPath) + strName;
            AddArchive(strPath.c_str());
        }
    }
}

std::vector<std::string> CVirtualFileSystem::FindFiles(const char *pStr, const char *pExt) const
{
    std::vector<std::string> Result;
    unsigned cchExt = pExt ? strlen(pExt) : 0;
    
    for(std::map<std::string, std::string>::const_iterator it = m_FileNameToArchive.begin();
        it != m_FileNameToArchive.end();
        ++it)
    {
        if(pStr && !StrStrI(it->first.c_str(), pStr))
            continue;
        if(pExt && (it->first.size() < cchExt || StrCmpI(it->first.c_str() + it->first.size() - cchExt, pExt) != 0))
            continue;
        
        Result.push_back(it->first);
    }
    
    return Result;
}

bool CVirtualFileSystem::DoesFileExists(const char *pStr) const
{
    string strFilenameLower;
    while(*pStr)
        strFilenameLower += tolower(*(pStr++));
    
    std::map<std::string, std::string>::const_iterator it;
    it = m_FileNameToArchive.find(strFilenameLower);
    return it != m_FileNameToArchive.end();
}

streampos CVfsFileBuf::seekpos(streampos nPos, ios_base::openmode Which)
{
    if(!m_pFile || Which != ios_base::in || nPos > m_nSize || nPos < 0)
        return -1;
    
    if(fseek(m_pFile, (long)(nPos - m_nPos), SEEK_CUR) == 0)
    {
        m_nPos = nPos;
        setg(NULL, NULL, NULL);
    }
    
    return m_nPos;
}

streampos CVfsFileBuf::seekoff(streamoff nOffset, ios_base::seekdir Way, ios_base::openmode Which)
{
    streampos nNewPos;
    if(Way == ios_base::beg)
        nNewPos = nOffset;
    else if(Way == ios_base::cur)
        nNewPos = m_nPos - (streampos)(egptr() - gptr()) + nOffset;
    else if(Way == ios_base::end)
        nNewPos = m_nSize - nOffset;
    
    return seekpos(nNewPos, Which);
}

int CVfsFileBuf::underflow()
{
    if(!m_pFile || m_nPos >= m_nSize)
        return EOF;
    
    streamsize cBytes = sizeof(m_Buf);
    if((streamsize)m_nPos + (streamsize)cBytes > m_nSize)
        cBytes = m_nSize - m_nPos;
    
    cBytes = fread(m_Buf, 1, (size_t)cBytes, m_pFile);
    m_nPos += cBytes;
    setg(m_Buf, m_Buf, m_Buf + cBytes);
    
    if(!cBytes)
    {
        printf("EOF!!!!!!!!\n");
        return EOF;
    }
        
    
    return (unsigned char)*gptr();
}
