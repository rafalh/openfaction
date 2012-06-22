/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CLevelChangePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CLEVELCHANGEPACKET_H
#define CLEVELCHANGEPACKET_H

#include "main.h"
#include "CPlayer.h"

class CLevelChangePacket
{
    public:
        CLevelChangePacket(const char *pLevelFileName, uint32_t nLevelChecksum):
            m_pLevelFileName(pLevelFileName), m_nLevelChecksum(nLevelChecksum) {}
        
        inline void Send(CPlayer *pPlayer) const
        {
            COutputBinaryStringStream Stream;
            
            Stream.WriteString(m_pLevelFileName);
            Stream.WriteUInt32(m_nLevelChecksum);
            
            pPlayer->GetConnection()->AddPacket(RF_LEVEL_CHANGE, Stream.str(), true);
        }
    
    private:
        const char *m_pLevelFileName;
        uint32_t m_nLevelChecksum;
};

#endif // CLEVELCHANGEPACKET_H
