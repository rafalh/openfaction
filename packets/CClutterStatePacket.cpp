/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CClutterStatePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CClutterStatePacket.h"
#include "CServer.h"
#include "CClutter.h"
#include "CLevel.h"
#include "CGame.h"
#include "CElementsMgr.h"

using namespace std;

void CClutterStatePacket::Send(CPlayer *pPlayer) const
{
    const set<CElement*> Clutter = pPlayer->GetServer()->GetGame()->GetLevel()->GetElementsMgr()->GetByType(OFET_CLUTTER);
    CString ClutterBitmap;
    unsigned cClutter = 0;
    for(set<CElement*>::iterator it = Clutter.begin(); it != Clutter.end(); ++it)
    {
        const CClutter * const pClutter = (CClutter*)*it;
        if(pClutter->GetLife() >= 0.0f)
        {
            ClutterBitmap.resize(cClutter / 8 + 1);
            
            if(pClutter->GetLife() > 0.0f)
                ClutterBitmap[cClutter / 8] |= 1 << (cClutter % 8);
            
            ++cClutter;
        }
    }
    
    if(cClutter > 0)
    {
        COutputBinaryStringStream Stream;
        Stream.WriteUInt32(cClutter);
        Stream.WriteBinary(ClutterBitmap);
        pPlayer->GetConnection()->AddPacket(RF_CLUTTER_STATE, Stream.str(), true);
    }
}
