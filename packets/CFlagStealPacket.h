/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagStealPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFLAGSTEALPACKET_H
#define CFLAGSTEALPACKET_H

#include "CPlayer.h"

class CCaptureTheFlag;

class CFlagStealPacket
{
    public:
        inline CFlagStealPacket(CCaptureTheFlag *pCtf, CPlayer *pPlayer):
            m_pCtf(pCtf), m_pPlayer(pPlayer) {}
        
        void Send(CPlayer *pPlayer) const;
        
    private:
        CCaptureTheFlag *m_pCtf;
        CPlayer *m_pPlayer;
};

#endif // CFLAGSTEALPACKET_H
