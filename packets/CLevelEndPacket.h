/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CLevelEndPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CLEVELENDPACKET_H
#define CLEVELENDPACKET_H

#include "CPlayer.h"

class CLevelEndPacket
{
    public:
        inline void Send(CPlayer *pPlayer) const
        {
            pPlayer->GetConnection()->AddPacket(RF_LEVEL_END, "", true);
        }
};

#endif // CLEVELENDPACKET_H
