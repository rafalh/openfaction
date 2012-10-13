/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CTblReader.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CTBLREADER_H
#define CTBLREADER_H

#include "CString.h"

class CStringsTable;

class CTblReader
{
    public:
        CTblReader(std::istream &Stream, const char *pSectionName = NULL);
        
        inline bool IsSectionFound() const
        {
            return m_bSectionFound;
        }
        
        inline const char *GetName() const
        {
            return m_strBuf.c_str() + m_nNamePos;
        }
        
        int LoadNextElement();
        int LoadNextLine();
        void GetString(CString &strRet, const CStringsTable *pStrTbl = NULL);
        void GetInt(int &iRet);
        void GetUInt(unsigned &nRet);
        void GetFloat(float &fRet);
        void GetBool(bool &bRet);
        void GetFlags(unsigned &nRet, const char *pFlags[], unsigned cFlags);
        void GetEnum(int &nRet, const char *pNames[], unsigned cNames);
    
    private:
        inline void IgnoreSpaces()
        {
            while(m_nPos < m_strBuf.size() && isspace(m_strBuf[m_nPos]))
                ++m_nPos;
        }
        
        std::istream &m_Stream;
        bool m_bSectionFound;
        CString m_strSection;
        unsigned m_iLine;
        size_t m_nPos, m_nNamePos;
        CString m_strBuf;
};

#endif // CTBLREADER_H
