/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CServer.h
*  PURPOSE:     Main server class
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSERVER_H
#define CSERVER_H

#include "CSocket.h"
#include "precomp.h"
#include "main.h"
#include "CThread.h"
#include "CTimer.h"
#include "CPlayersManager.h"
#include "CInputBinaryStream.h"
#include "CChecksum.h"
#include "rfproto.h"
#include "CGame.h"
#include "CEventsHandler.h"
#include "CException.h"

class CPlayer;
class CClutter;
class CItem;
class CRoom;
class CTrigger;
class CGame;
class CServerConfig;
class CBanlist;
class CGamemode;
struct SMpCharacter;
struct SDamageInfo;

class CServer: public CEventsHandler
{
    public:
        CServer(CServerConfig *pServerConfig);
        ~CServer();
        void Init() throw(CException);
        void Start(bool bBlock);
        void Stop();
        void LoadLevel(const char *pFileName) throw(CException);
        void LoadNextLevel() throw(CException);
        void SendGlobalMessage(const char *pMsg) const;
        
        inline CSocket &GetSocket()
        {
            return m_Sock;
        }
        
        inline CGame *GetGame()
        {
            return m_pGame;
        }
        
        inline CLevel *GetLevel()
        {
            return m_pGame->GetLevel();
        }
        
        inline const char *GetLevelFileName() const
        {
            return m_strLevelFileName.c_str();
        }
        
        inline unsigned GetLevelTime() const
        {
            return m_LevelTimer.GetValue();
        }
        
        inline void QueueLevelChange(const char *pLevelFileName)
        {
            m_strQueuedLevelChange = pLevelFileName;
        }

        inline CServerConfig *GetConf()
        {
            return m_pConf;
        }
        
        CBanlist *GetBanlist()
        {
            return m_pBanlist;
        }
        
        inline const CPlayersManager *GetPlayersMgr() const
        {
            return &m_PlayersMgr;
        }
        
        inline const CGamemode *GetGamemode() const
        {
            return m_pGamemode;
        }
        
        void KickPlayer(CPlayer *pPlayer, rfLeftReason nReason = RF_LR_KICKED, bool bTimedOut = false);
        int SpawnPlayer(CPlayer *pPlayer, const SMpCharacter *pCharacter, btVector3 *pPos = NULL);
        
        // Events
        void OnTiggerActivate(CTrigger *pTrigger, CEntity *pEntity);
        bool OnItemApply(CItem *pItem, CEntity *pEntity);
        void OnClutterKill(CClutter *pClutter, SDamageInfo &DmgInfo);
        void OnEntityKill(CEntity *pEntity, SDamageInfo &DmgInfo);
        void OnGlassKill(CRoom *pRoom, SDamageInfo &DmgInfo);

    private:
        void ProcessGamePacket(unsigned nType, CInputBinaryStream &Stream, const CSocketAddress &Addr, unsigned cBytes);
        void ProcessGamePackets(CInputBinaryStream &Stream, const CSocketAddress &Addr);
        void ProcessSyncPacket(CInputBinaryStream &Stream, const CSocketAddress &Addr);
        static DECLARE_THREAD_FUNC(RecvThreadProcStatic, param);
        static DECLARE_THREAD_FUNC(PingThreadProcStatic, param);
        void RecvThreadProc();
        void PingThreadProc();
        void PingTracker();
        void PingPlayers();
        void KickTimedOutPlayers();
        void SendUpdateItemsPacket();
        void SendUpdateEntitiesPacket();
        void SendStatsPacket();
        void FinishLevelChange();
        void FlushPackets();
        
        CPlayersManager m_PlayersMgr;
        CGame *m_pGame;
        CServerConfig *m_pConf;
        CBanlist *m_pBanlist;
        CSocket m_Sock;
        
        CThread m_MainThread;
        CThread m_RecvThread;
        
        CString m_strLevelFileName;
        CString m_strQueuedLevelChange;
        CTimer m_LevelTimer;
        time_t m_LoadingLevelTimestamp;
        unsigned m_nCurrentLevel;
        
        CChecksum m_TablesVppChecksum;
        unsigned m_cbTablesVpp;
        
        unsigned m_nBluePlayers;
        
        CGamemode *m_pGamemode;
};

#endif // CSERVER_H
