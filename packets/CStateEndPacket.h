/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CStateEndPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSTATEENDPACKET_H
#define CSTATEENDPACKET_H

#include "precomp.h"
#include "formats/rfproto.h"
#include "CPlayer.h"

class CStateEndPacket
{
    public:
        inline void Send(CPlayer *pPlayer) const
        {
            pPlayer->GetConnection()->AddPacket(RF_STATE_END, "", true);
        }
};

#endif // CSTATEENDPACKET_H
