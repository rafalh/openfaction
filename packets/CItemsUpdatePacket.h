/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CItemsUpdatePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CITEMSUPDATEPACKET_H
#define CITEMSUPDATEPACKET_H

#include "CPlayer.h"

class CItemsUpdatePacket
{
    public:
        void Send(CPlayer *pPlayer) const;
};

#endif // CITEMSUPDATEPACKET_H
