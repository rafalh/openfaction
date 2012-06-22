/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagsStatePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFLAGSSTATEPACKET_H
#define CFLAGSSTATEPACKET_H

#include "CServer.h"

class CCaptureTheFlag;

class CFlagsStatePacket
{
    public:
        inline CFlagsStatePacket(CCaptureTheFlag *pCtf):
            m_pCtf(pCtf) {}
        void Send(CPlayer *pPlayer) const;
    
    private:
        CCaptureTheFlag *m_pCtf;
};

#endif // CFLAGSSTATEPACKET_H
