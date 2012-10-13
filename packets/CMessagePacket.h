/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CMessagePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CMESSAGEPACKET_H
#define CMESSAGEPACKET_H

#include "precomp.h"
#include "CPlayer.h"
#include "CInputBinaryStream.h"

class CMessagePacket
{
    public:
        inline CMessagePacket(const char *pMessage, CPlayer *pPlayer = NULL, bool bTeamMsg = false):
            m_strMessage(pMessage), m_pPlayer(pPlayer), m_bTeamMsg(bTeamMsg) {}
        
        CMessagePacket(CServer *pServer, CInputBinaryStream &Stream);
        void Send(CPlayer *pPlayer) const;
        
        inline const char *GetMessage() const
        {
            return m_strMessage.c_str();
        }
        
        inline CPlayer *GetPlayer() const
        {
            return m_pPlayer;
        }
        
        inline bool IsTeamMsg() const
        {
            return m_bTeamMsg;
        }
        
    private:
        CString m_strMessage;
        CPlayer *m_pPlayer;
        bool m_bTeamMsg;
};

#endif // CMESSAGEPACKET_H
