/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagReturnPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFLAGRETURNPACKET_H
#define CFLAGRETURNPACKET_H

#include "CServer.h"
#include "CPlayer.h"

class CCaptureTheFlag;

class CFlagReturnPacket
{
   public:
        inline CFlagReturnPacket(CCaptureTheFlag *pCtf, bool bBlueFlag, CPlayer *pPlayer = NULL):
            m_pCtf(pCtf), m_bBlueFlag(bBlueFlag), m_pPlayer(pPlayer) {}
        
        void Send(CPlayer *pPlayer) const;
        
    private:
        CCaptureTheFlag *m_pCtf;
        bool m_bBlueFlag;
        CPlayer *m_pPlayer;
};

#endif // CFLAGRETURNPACKET_H
