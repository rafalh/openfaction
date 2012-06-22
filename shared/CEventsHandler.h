/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CEventsHandler.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CEVENTSHANDLER_H
#define CEVENTSHANDLER_H

class CTrigger;
class CEntity;
class CItem;
class CClutter;
class CRoom;
struct SDamageInfo;

class CEventsHandler
{
    public:
        virtual ~CEventsHandler() {}
        
        virtual void OnTiggerActivate(CTrigger *pTrigger, CEntity *pEntity) {}
        virtual bool OnItemApply(CItem *pItem, CEntity *pEntity) { return false; }
        virtual void OnClutterKill(CClutter *pClutter, SDamageInfo &DmgInfo) {}
        virtual void OnEntityKill(CEntity *pEntity, SDamageInfo &DmgInfo) {}
        virtual void OnGlassKill(CRoom *pRoom, SDamageInfo &DmgInfo) {}
        
        static CEventsHandler Default;
};

#endif // CEVENTSHANDLER_H
