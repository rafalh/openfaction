/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CFileFactory.h
*  PURPOSE:     irrklang::IFileFactory implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFILEFACTORY_H
#define CFILEFACTORY_H

#include <irrKlang.h>
#include "CString.h"
#include "CVirtualFileSystem.h"

class CIrrKlangReadFile: public irrklang::IFileReader
{
    public:
        CIrrKlangReadFile(const char *pszFilename):
            m_File(pszFilename), m_strPath(pszFilename) {}
        
        ~CIrrKlangReadFile()
        {
            m_File.Close();
        }
        
        irrklang::ik_s32 read(void* buffer, irrklang::ik_u32 sizeToRead)
        {
            m_File.read((char*)buffer, sizeToRead);
            return m_File ? sizeToRead : 0;
        }
        
        bool seek(irrklang::ik_s32 finalPos, bool relativeMovement = false)
        {
            return (bool) m_File.seekg(finalPos, relativeMovement ? std::istream::cur : std::istream::beg);
        }
        
        irrklang::ik_s32 getSize()
        {
            return m_File.GetSize();
        }
        
        irrklang::ik_s32 getPos()
        {
            return (irrklang::ik_s32)const_cast<CVfsFileStream&>(m_File).tellg();
        }
        
        const irrklang::ik_c8 *getFileName()
        {
            return m_strPath.c_str();
        }
    
    private:
        CVfsFileStream m_File;
        const CString m_strPath;
};


class CIrrKlangFileFactory: public irrklang::IFileFactory
{
    public:
        irrklang::IFileReader *createFileReader(const irrklang::ik_c8 *pszFilename)
        {
            return new CIrrKlangReadFile(pszFilename);
        }
    
    private:
};

#endif // CFILEFACTORY_H
