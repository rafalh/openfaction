/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CGlassStatePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CGlassStatePacket.h"
#include "CServer.h"
#include "CStaticGeometry.h"
#include "CLevel.h"
#include "CElementsMgr.h"

using namespace std;

void CGlassStatePacket::Send(CPlayer *pPlayer) const
{
    string RoomsBitmap;
    
    const set<CElement*> Rooms = pPlayer->GetServer()->GetLevel()->GetElementsMgr()->GetByType(OFET_LEVEL);
    set<CElement*>::iterator it;
    for(it = Rooms.begin(); it != Rooms.end(); ++it)
    {
        CRoom *pRoom = (CRoom*)*it;
        
        if(pRoom->IsGlass())
        {
            unsigned i = pRoom->GetGlassIndex();
            
            if(i / 8 >= RoomsBitmap.size())
                RoomsBitmap.resize(i / 8 + 1);
            
            if(pRoom->GetLife() > 0.0f)
                RoomsBitmap[i / 8] |= 1 << (i % 8);
        }
    }
    
    if(!RoomsBitmap.empty())
        pPlayer->GetConnection()->AddPacket(RF_GLASS_STATE, RoomsBitmap, true);
}
