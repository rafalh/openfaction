/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CSpawnRequestPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSPAWNREQUESTPACKET_H
#define CSPAWNREQUESTPACKET_H

#include "CPlayer.h"
#include "CInputBinaryStream.h"
#include "CMpCharactersTable.h"
#include "CServer.h"

class CSpawnRequestPacket
{
    public:
        inline CSpawnRequestPacket(CServer *pServer, CInputBinaryStream &Stream)
        {
            m_pCharacter = pServer->GetGame()->GetMpCharactersTbl()->Get(Stream.ReadUInt32());
            m_pPlayer = pServer->GetPlayersMgr()->Get(Stream.ReadUInt8());
        }
        
        inline const SMpCharacter *GetCharacter() const
        {
            return m_pCharacter;
        }
        
        inline CPlayer *GetPlayer() const
        {
            return m_pPlayer;
        }
    
    private:
        const SMpCharacter *m_pCharacter;
        CPlayer *m_pPlayer;
};

#endif // CSPAWNREQUESTPACKET_H
