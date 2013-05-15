/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/gamemodes/CGamemode.h
*  PURPOSE:     Base class for gamemodes
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CGAMEMODE_H
#define CGAMEMODE_H

class CServer;
class CEntity;
class CPlayer;
class CItem;
struct SDamageInfo;

class CGamemode
{
    public:
        CGamemode(CServer *pServer):
            m_pServer(pServer) {}
        virtual ~CGamemode() {}
        
        virtual unsigned GetId() const = 0;
        virtual void Update(unsigned nDeltaTime) {}
        virtual void OnEntityKill(CEntity *pEntity, SDamageInfo &DmgInfo);
        virtual void OnPlayerKill(CPlayer *pPlayer, CPlayer *pKiller);
        virtual bool OnItemApply(CItem *pItem, CEntity *pEntity) { return true; }
        virtual void OnPlayerJoin(CPlayer *pPlayer) {}
        virtual void OnPlayerLeft(CPlayer *pPlayer) {}
        
        virtual unsigned GetRedScore() { return 0; }
        virtual unsigned GetBlueScore() { return 0; }
        
        static CGamemode *Create(CServer *pServer, unsigned Id);
    
    protected:
        CServer *m_pServer;
};

#endif // CGAMEMODE_H
