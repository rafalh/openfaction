/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CJoinSuccessPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef _CJOINSUCCESSPACKET_H
#define _CJOINSUCCESSPACKET_H

#include "CPlayer.h"

class CJoinSuccessPacket
{
    public:
        void Send(CPlayer *pPlayer) const;
};

#endif // _CJOINSUCCESSPACKET_H
