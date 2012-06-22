/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CEntityUpdatePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CENTITYUPDATEPACKET_H
#define CENTITYUPDATEPACKET_H

#include "precomp.h"
#include "CPlayer.h"
#include "CEntity.h"
#include "CInputBinaryStream.h"

class CEntityUpdatePacket
{
    public:
        inline void Add(CEntity *pEntity)
        {
            m_Entities.push_back(pEntity);
        }
        
        void Send(CPlayer *pPlayer, bool bSyncAll = false) const;
        static void Process(CInputBinaryStream &Stream, CPlayer *pPlayer);
    
    private:
        std::vector<CEntity*> m_Entities;
};

#endif // CENTITYUPDATEPACKET_H
