/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CTblReader.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "utils.h"
#include "CTblReader.h"
#include "CStringsTable.h"
#include "CException.h"
#include <cstdlib>
#include <iostream>

using namespace std;

CTblReader::CTblReader(istream &Stream, const char *pSectionName):
    m_Stream(Stream), m_bSectionFound(false), m_iLine(0), m_nPos(0), m_nNamePos(0)
{
    if(!pSectionName)
        m_bSectionFound = true;
    else
        m_strSection = pSectionName;
    
    while(m_Stream.good() && !m_bSectionFound)
    {
        /* Get line from file */
        CString strLine;
        getline(m_Stream, strLine);
        ++m_iLine;
        
        /* Hack: remove \r from the end of line */
        if(strLine.size() > 0 && strLine[strLine.size() - 1] == '\r')
            strLine.resize(strLine.size() - 1);
        
        /* Check if we found the section */
        if(strLine.size() > 0 && strLine[0] == '#' && !strLine.comparei(pSectionName, 1))
            m_bSectionFound = true;
    }
}

int CTblReader::LoadNextLine()
{
    while(m_Stream.good())
    {
        getline(m_Stream, m_strBuf);
        ++m_iLine;
        
        size_t nCommentPos = m_strBuf.find("//");
        if(nCommentPos != m_strBuf.npos)
            m_strBuf.resize(nCommentPos);
        
        for(m_nPos = 0; m_nPos < m_strBuf.size() && isspace(m_strBuf[m_nPos]); ++m_nPos);
        
        if(m_nPos < m_strBuf.size() && !isspace(m_strBuf[m_nPos]))
            return 0;
    }
    
    return -1;
}

int CTblReader::LoadNextElement()
{
    if(!m_bSectionFound)
        return -1;
    
    if(LoadNextLine() < 0)
        return -1;
    
    m_nNamePos = m_nPos;
    
    if(!strcmp(m_strBuf.c_str() + m_nNamePos, "#End"))
        return -1;
    
    m_nPos = m_strBuf.find(':', m_nNamePos);
    if(m_nPos != m_strBuf.npos)
    {
        m_strBuf[m_nPos] = '\0';
        ++m_nPos;
    }
    else
        m_nPos = m_strBuf.size();
    
    return 0;
}

void CTblReader::GetString(CString &strRet, const CStringsTable *pStrTbl)
{
    IgnoreSpaces();
    
    if(m_nPos >= m_strBuf.size())
        THROW_EXCEPTION("Invalid string at line %u (%s)", m_iLine, m_strSection.c_str());
    
    if(pStrTbl && m_strBuf.size() - m_nPos > 4 && !strncmp(m_strBuf.c_str() + m_nPos, "XSTR", 4))
    {
        /* Handle XSTR macro */
        
        m_nPos += 4;
        IgnoreSpaces();
        
        if(m_strBuf[m_nPos] != '(')
            THROW_EXCEPTION("Expected '(', got '%c' at line %u (%s)", m_strBuf[m_nPos], m_iLine, m_strSection.c_str());
        
        ++m_nPos;
        IgnoreSpaces();
        
        char *pszEnd;
        unsigned iStr = strtoul(m_strBuf.c_str() + m_nPos, &pszEnd, 0);
        if(!iStr)
            THROW_EXCEPTION("Invalid XSTR at line %u (%s)", m_iLine, m_strSection.c_str());
        
        m_nPos += pszEnd - (m_strBuf.c_str() + m_nPos);
        IgnoreSpaces();
        
        CString strDefault;
        if(m_strBuf[m_nPos] == ',')
        {
            ++m_nPos;
            GetString(strDefault);
        }
        
        IgnoreSpaces();
        if(m_strBuf[m_nPos] != ')')
            THROW_EXCEPTION("Expected ')', got '%c' at line %u (%s)", m_strBuf[m_nPos], m_iLine, m_strSection.c_str());
        
        ++m_nPos;
        
        const char *pszResult = pStrTbl->Get(iStr);
        strRet = pszResult ? CString(pszResult) : strDefault;
        
        return;
    }
    
    if(m_strBuf[m_nPos] != '"')
        THROW_EXCEPTION("Expected '\"', got '%c' at line %u (%s)", m_strBuf[m_nPos], m_iLine, m_strSection.c_str());
    
    ++m_nPos;
    size_t nEndPos = m_strBuf.find('"', m_nPos);
    if(nEndPos == m_strBuf.npos)
        THROW_EXCEPTION("Invalid string at line %u (%s)", m_iLine, m_strSection.c_str());
    
    strRet = m_strBuf.substr(m_nPos, nEndPos - m_nPos);
    m_nPos = nEndPos + 1;
}

void CTblReader::GetInt(int &iRet)
{
    IgnoreSpaces();
    
    const char *pBegin = m_strBuf.c_str() + m_nPos, *pEnd = pBegin;
    int iRet2 = strtol(pBegin, (char**)&pEnd, 0);
    if(pEnd < m_strBuf.c_str() + m_strBuf.size() && !isspace(*pEnd))
        THROW_EXCEPTION("Invalid integer at line %u", m_iLine);
    
    m_nPos += pEnd - pBegin;
    iRet = iRet2;
}

void CTblReader::GetUInt(unsigned &nRet)
{
    IgnoreSpaces();
    
    const char *pBegin = m_strBuf.c_str() + m_nPos, *pEnd = pBegin;
    unsigned nRet2 = strtoul(pBegin, (char**)&pEnd, 0);
    if(pEnd < m_strBuf.c_str() + m_strBuf.size() && !isspace(*pEnd))
        THROW_EXCEPTION("Invalid unsigned integer at line %u", m_iLine);
    
    nRet = nRet2;
    m_nPos += pEnd - pBegin;
}

void CTblReader::GetFloat(float &fRet)
{
    IgnoreSpaces();
    
    const char *pBegin = m_strBuf.c_str() + m_nPos, *pEnd = pBegin;
    float fRet2 = (float)strtod(pBegin, (char**)&pEnd);
    if(pEnd < m_strBuf.c_str() + m_strBuf.size() && !isspace(*pEnd))
        THROW_EXCEPTION("Invalid float at line %u", m_iLine);
    
    fRet = fRet2;
    m_nPos += pEnd - pBegin;
}
        
void CTblReader::GetBool(bool &bRet)
{
    IgnoreSpaces();
    
    bool bRet2;
    if(!m_strBuf.compare(m_nPos, 4, "true"))
        bRet2 = true;
    else if(!m_strBuf.compare(m_nPos, 5, "false"))
        bRet2 = false;
    else
        THROW_EXCEPTION("Invalid bool at line %u", m_iLine);
    
    size_t nNewPos = m_nPos + (bRet2 ? 4 : 5);
    if(nNewPos < m_strBuf.size() && !isspace(m_strBuf[nNewPos]))
        THROW_EXCEPTION("Invalid bool at line %u", m_iLine);
    
    bRet = bRet2;
    m_nPos = nNewPos;
}

void CTblReader::GetFlags(unsigned &nRet, const char *pFlags[], unsigned cFlags)
{
    IgnoreSpaces();
    
    if(m_nPos >= m_strBuf.size() || m_strBuf[m_nPos] != '(')
        THROW_EXCEPTION("Invalid flags at line %u (%s)", m_iLine, m_strSection.c_str());
    
    ++m_nPos;
    
    unsigned nFlags = 0;
    CString strFlag;
    while(m_strBuf[m_nPos] != ')')
    {
        GetString(strFlag);
        IgnoreSpaces();
        
        unsigned i;
        for(i = 0; i < cFlags; ++i)
            if(strFlag == pFlags[i])
            {
                nFlags |= (1 << i);
                break;
            }
        
        // Flag not found
        if(i >= cFlags)
            THROW_EXCEPTION("Invalid flag at line %u (%s)", m_iLine, m_strSection.c_str());
    }
    
    if(m_nPos >= m_strBuf.size() || m_strBuf[m_nPos] != ')')
        THROW_EXCEPTION("Invalid flags at line %u (%s)", m_iLine, m_strSection.c_str());
    
    nRet = nFlags;
    m_nPos += 1;
}

void CTblReader::GetEnum(int &nRet, const char *pNames[], unsigned cNames)
{
    CString strName;
    GetString(strName);
    
    for(unsigned i = 0; i < cNames; ++i)
        if(strName == pNames[i])
        {
            nRet = i;
            return;
        }
    
    // Name not found
    THROW_EXCEPTION("Invalid enum value at line %u", m_iLine);
}
