/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CClutterStatePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCLUTTERSTATEPACKET_H
#define CCLUTTERSTATEPACKET_H

#include "CPlayer.h"

class CClutterStatePacket
{
    public:
        void Send(CPlayer *pPlayer) const;
};

#endif // CCLUTTERSTATEPACKET_H
