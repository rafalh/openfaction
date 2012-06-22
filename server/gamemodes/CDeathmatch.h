/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/gamemodes/CDeathmatch.h
*  PURPOSE:     Deathmatch gamemode implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CDEATHMATCH_H
#define CDEATHMATCH_H

#include "gamemodes/CGamemode.h"
#include "rfproto.h"

class CDeathmatch: public CGamemode
{
    public:
        CDeathmatch(CServer *pServer):
            CGamemode(pServer) {}
        
        virtual void OnPlayerKill(CPlayer *pPlayer, CPlayer *pKiller);
        
        unsigned GetId() const
        {
            return RF_DM;
        }
        
        static const char *GetName()
        {
            return "dm";
        }
};

#endif // CDEATHMATCH_H
