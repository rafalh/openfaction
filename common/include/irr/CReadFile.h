/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CReadFile.h
*  PURPOSE:     Implementation of irr::io::IReadFile for reading VPP files
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CIRRREADFILE_H
#define CIRRREADFILE_H

#include <irrlicht.h>
#include "CVirtualFileSystem.h"

class CIrrReadFile: public irr::io::IReadFile
{
    public:
        CIrrReadFile(const char *pszFilename):
            m_File(pszFilename), m_strPath(pszFilename) {}
        
        ~CIrrReadFile()
        {
            m_File.Close();
        }
        
        irr::s32 read(void* buffer, irr::u32 sizeToRead)
        {
            m_File.read((char*)buffer, sizeToRead);
            return m_File ? sizeToRead : 0;
        }
        
        bool seek(long finalPos, bool relativeMovement = false)
        {
            return (bool) m_File.seekg(finalPos, relativeMovement ? std::istream::cur : std::istream::beg);
        }
        
        long getSize() const
        {
            return m_File.GetSize();
        }
        
        long getPos() const
        {
            return (long)const_cast<CVfsFileStream&>(m_File).tellg();
        }
        
        const irr::io::path& getFileName() const
        {
            return m_strPath;
        }
    
    private:
        CVfsFileStream m_File;
        const irr::io::path m_strPath;
};

#endif // CIRRREADFILE_H
