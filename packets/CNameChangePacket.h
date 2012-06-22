/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CNameChangePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CNAMECHANGEPACKET_H
#define CNAMECHANGEPACKET_H

#include "CPlayer.h"
#include "CInputBinaryStream.h"

class CNameChangePacket
{
    public:
        inline CNameChangePacket(CPlayer *pPlayer):
            m_pPlayer(pPlayer), m_strName(pPlayer->GetName()) {}
        
        CNameChangePacket(CServer *pServer, CInputBinaryStream &Stream);
        void Send(CPlayer *pPlayer) const;
        
        inline CPlayer *GetPlayer() const
        {
            return m_pPlayer;
        }
        
        inline const char *GetName() const
        {
            return m_strName.c_str();
        }
        
    private:
        CPlayer *m_pPlayer;
        std::string m_strName;
};

#endif // CNAMECHANGEPACKET_H
