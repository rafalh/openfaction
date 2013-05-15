/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagCapturePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFLAGCAPTUREPACKET_H
#define CFLAGCAPTUREPACKET_H

#include "CServer.h"

class CCaptureTheFlag;

class CFlagCapturePacket
{
    public:
        inline CFlagCapturePacket(CCaptureTheFlag *pCtf, CPlayer *pPlayer):
            m_pPlayer(pPlayer), m_pCtf(pCtf) {}
        
        void Send(CPlayer *pPlayer) const;
        
    private:
        CPlayer *m_pPlayer;
        CCaptureTheFlag *m_pCtf;
};

#endif // CFLAGCAPTUREPACKET_H
