/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CGlassStatePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CGLASSSTATEPACKET_H
#define CGLASSSTATEPACKET_H

#include "CPlayer.h"

class CGlassStatePacket
{
    public:
        void Send(CPlayer *pPlayer) const;
};

#endif // CGLASSSTATEPACKET_H
