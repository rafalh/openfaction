/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CPlayerLeftPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CPLAYERLEFTPACKET_H
#define CPLAYERLEFTPACKET_H

#include "CPlayer.h"
#include "CInputBinaryStream.h"
#include "rfproto.h"

class CPlayerLeftPacket
{
    public:
        inline CPlayerLeftPacket(CPlayer *pPlayer, rfLeftReason nReason = RF_LR_NORMAL):
            m_pPlayer(pPlayer), m_nReason(nReason) {}
        
        CPlayerLeftPacket(CServer *pServer, CInputBinaryStream &Stream);
        void Send(CPlayer *pPlayer) const;
        
        inline CPlayer *GetPlayer() const
        {
            return m_pPlayer;
        }
        
        inline rfLeftReason GetReason() const
        {
            return m_nReason;
        }
        
    private:
        CPlayer *m_pPlayer;
        rfLeftReason m_nReason;
};

#endif // CPLAYERLEFTPACKET_H
