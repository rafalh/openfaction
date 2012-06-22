/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CChecksum.cpp
*  PURPOSE:     Checksum algorithm used by RF
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CChecksum.h"

uint32_t CChecksum::m_Table[256];
bool CChecksum::m_bInitTable = true;

uint32_t CChecksum::Compute(std::istream &Stream)
{
    if(m_bInitTable)
    {
        for(uint32_t i = 0; i < 256; ++i) {
            m_Table[i] = i;
            for(unsigned j = 0; j < 8; j++) {
                if(m_Table[i] & 1)
                    m_Table[i] = (m_Table[i] >> 1) ^ 0xEDB88320;
                else
                    m_Table[i] = m_Table[i] >> 1;
            }
        }
        m_bInitTable = false;
    }
    
    m_nChecksum = 0;
    while(Stream.good())
    {
        uint8_t nByte = Stream.get();
        
        if(!Stream.good())
            break;
        
        m_nChecksum = m_Table[nByte ^ (m_nChecksum & 0xff)] ^ (m_nChecksum >> 8);
    }
    
    return m_nChecksum;
}
