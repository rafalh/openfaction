/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CChecksum.h
*  PURPOSE:     Checksum algorithm used by RF
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCHECKSUM_H
#define CCHECKSUM_H

#include <iostream>
#include <stdint.h>

class CChecksum
{
    public:
        inline CChecksum():
            m_nChecksum(0) {}
        
        uint32_t Compute(std::istream &Stream);
        
        inline uint32_t GetValue() const
        {
            return m_nChecksum;
        }
    
    private:
        uint32_t m_nChecksum;
        static uint32_t m_Table[256];
        static bool m_bInitTable;
};

#endif // CCHECKSUM_H
