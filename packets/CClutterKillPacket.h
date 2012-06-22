/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CClutterKillPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal
*
*****************************************************************************/

#ifndef CCLUTTERKILLPACKET_H
#define CCLUTTERKILLPACKET_H
/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/


#include "CClutter.h"
#include "CPlayer.h"

class CClutterKillPacket
{
    public:
        inline CClutterKillPacket(CClutter *pClutter):
            m_pClutter(pClutter) {}
        
        void Send(CPlayer *pPlayer) const;
    
    private:
        CClutter *m_pClutter;
};

#endif // CCLUTTERKILLPACKET_H
