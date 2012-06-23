/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CServer.cpp
*  PURPOSE:     Main server class
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "precomp.h"
#include "main.h"
#include "errors.h"
#include "CServer.h"
#include "CTimer.h"
#include "CPlayersManager.h"
#include "CLogger.h"
#include "CServerConfig.h"
#include "CLock.h"
#include "CFileList.h"
#include "CBanlist.h"
#include "CLevel.h"
#include "CStringsTable.h"
#include "CGameTable.h"
#include "CSpawnpoint.h"
#include "CStaticGeometry.h"
#include "CColWorld.h"
#include "CElementsMgr.h"
#include "CSpawnpointsManager.h"
#include "CVirtualFileSystem.h"
#include "CWeapon.h"
#include "gamemodes/CGamemode.h"
#include "CException.h"
#include "utils.h"

/* Packets */
#include "CClutterKillPacket.h"
#include "CClutterStatePacket.h"
#include "CEntityKillPacket.h"
#include "CEntitySpawnPacket.h"
#include "CEntityUpdatePacket.h"
#include "CFallDamagePacket.h"
#include "CGlassStatePacket.h"
#include "CItemApplyPacket.h"
#include "CItemSpawnPacket.h"
#include "CItemsUpdatePacket.h"
#include "CJoinFailedPacket.h"
#include "CJoinRequestPacket.h"
#include "CJoinSuccessPacket.h"
#include "CLevelChangePacket.h"
#include "CLevelEndPacket.h"
#include "CMessagePacket.h"
#include "CNameChangePacket.h"
#include "CNewPlayerPacket.h"
#include "CPingPacket.h"
#include "CPlayerStatsPacket.h"
#include "CPlayerLeftPacket.h"
#include "CPlayersListPacket.h"
#include "CRconRequestPacket.h"
#include "CRconCommandPacket.h"
#include "CReloadPacket.h"
#include "CReplyPacket.h"
#include "CGlassKillPacket.h"
#include "CServerEndPacket.h"
#include "CServerInfoPacket.h"
#include "CShootPacket.h"
#include "CSpawnRequestPacket.h"
#include "CStateEndPacket.h"
#include "CTeamChangePacket.h"
#include "CTriggerActivatePacket.h"

using namespace std;

CServer::CServer(CServerConfig *pServerConfig):
    m_PlayersMgr(this),
    m_LoadingLevelTimestamp(0), m_nCurrentLevel(0),
    m_nBluePlayers(0),
    m_pGame(NULL),
    m_pGamemode(NULL)
{
    m_pGame = new CGame(CLogger::GetInst().GetConsole());
    m_pConf = pServerConfig;
    m_pBanlist = new CBanlist(this);
}
        
inline CServer::~CServer()
{
    Stop();
    
    delete m_pGame;
    m_pGame = NULL;
    
    delete m_pConf;
    m_pConf = NULL;
    
    delete m_pBanlist;
    m_pBanlist = NULL;
}

void CServer::Init() throw(CException)
{
    CGame::SConfig GameConfig;
    GameConfig.bWeaponsStay = m_pConf->GetWeaponsStay();
    m_pGame->SetEventsHandler(this);
    m_pGame->SetConfig(GameConfig);
    
    /* Add VPP archives to filesystem */
    m_pGame->InitVfs();
    
    /* Load TBL files */
    m_pGame->LoadTables();
    
    CSocketAddress Addr((uint32_t)INADDR_ANY, m_pConf->GetPort());
    int iStatus;
    
    /* Calculate tables.vpp checksum and size - it's used to prevent people from hacking game */
    ifstream Stream("tables.vpp", ifstream::binary);
    m_TablesVppChecksum.Compute(Stream);
    Stream.clear();
    Stream.seekg(0, ifstream::end);
    m_cbTablesVpp = Stream.tellg();
    
    /* Create socket */
    iStatus = m_Sock.Create();
    if(iStatus < 0)
        THROW_EXCEPTION("Error %d! Cannot create socket.", m_Sock.GetLastError());
    
    /* Bind socket to local address */
    iStatus = m_Sock.Bind(Addr);
    if(iStatus < 0)
        THROW_EXCEPTION("Error %d! Cannot bind socket.", m_Sock.GetLastError());
    
    /* Init gamemode */
    m_pGamemode = CGamemode::Create(this, m_pConf->GetGameType());
    
    /* Load first level */
    const char * const pLevelFileName = m_pConf->GetLevel(m_nCurrentLevel);
    
    LoadLevel(pLevelFileName);
}

void CServer::Start(bool bBlock)
{
    /* Start main thread */
    int iStatus = m_MainThread.Start(PingThreadProcStatic, this);
    if(iStatus < 0)
        THROW_EXCEPTION("Failed to start main thread");
    
    /* Start recv thread and block current thread if requested */
    if(bBlock)
        RecvThreadProc();
    else
    {
        iStatus = m_RecvThread.Start(RecvThreadProcStatic, this);
        if(iStatus < 0)
            THROW_EXCEPTION("Failed to start recv thread");
    }
}

void CServer::Stop()
{
    g_Lock.Acquire();
    
    /* Terminate running threads */
    
    if(m_MainThread.IsRunning())
        m_MainThread.Terminate();
    
    if(m_RecvThread.IsRunning())
        m_RecvThread.Terminate();
    
    /* Inform players server has been stopped */
    CServerEndPacket ServEndPacket;
    CPlayer *pPlayer = m_PlayersMgr.Enum(NULL);
    while(pPlayer)
    {
        CPlayer *pNext = m_PlayersMgr.Enum(pPlayer);
        ServEndPacket.Send(pPlayer);
        m_PlayersMgr.Remove(pPlayer);
        pPlayer = pNext;
    }
    
    /* Inform tracker server has been stopped */
    
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(RF_TRACKER);
    Stream.WriteUInt8(0x06);                      // unknown
    Stream.WriteUInt16(RF_TRACKER_SERVER_STOPED); // type
    Stream.WriteUInt32(0);                        // seq
    Stream.WriteUInt16(0x000A);                   // packet_len
    Stream.WriteUInt8(0x00);                      // unknown2
    
    CSocketAddress Addr(RF_TRACKER_HOST, RF_TRACKER_PORT);
    m_Sock.SendTo(Stream.str(), Addr);
    
    g_Lock.Release();
}

void CServer::LoadLevel(const char *pszFilename) throw(CException)
{
    /* Load level */
    m_pGame->LoadLevel(pszFilename);
    
    /* Inform players level is changing */
    CLevelEndPacket LvlEndPacket;
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        LvlEndPacket.Send(pPlayer);
    
    m_strLevelFileName = pszFilename;
    m_LoadingLevelTimestamp = time(NULL);
}

void CServer::LoadNextLevel() throw(CException)
{
    ++m_nCurrentLevel;
    
    /* Found next level */
    const char *pLevelFileName = m_pConf->GetLevel(m_nCurrentLevel);
    if(!pLevelFileName)
    {
        m_nCurrentLevel = 0;
        pLevelFileName = m_pConf->GetLevel(m_nCurrentLevel);
    }
    
    /* Load next level */
    LoadLevel(pLevelFileName);
}

void CServer::SendGlobalMessage(const char *pMsg) const
{
    CMessagePacket MsgPacket(pMsg);
    
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        MsgPacket.Send(pPlayer);
    
    CLogger::GetInst().PrintStatus("Server: %s", pMsg);
}

void CServer::ProcessGamePacket(unsigned nType, CInputBinaryStream &Stream, const CSocketAddress &Addr, unsigned cBytes)
{
    CPlayer *pPlayer = m_PlayersMgr.Get(Addr);
    
    if(nType != RF_JOIN_REQUEST && nType != RF_SERVER_INFO_REQUEST && !pPlayer)
        return;
    
    if(pPlayer)
        pPlayer->m_Timestamp = time(NULL);
    
    switch(nType)
    {
        case RF_SERVER_INFO_REQUEST:
            CServerInfoPacket().Send(this, Addr);
            break;
        
        case RF_JOIN_REQUEST:
        {
            CJoinRequestPacket JoinReqPacket(Stream);
            
            if(pPlayer)
                CJoinFailedPacket(RF_JFR_THE_SAME_IP).Send(this, Addr);
            else if(m_PlayersMgr.GetCount() >= m_pConf->GetMaxPlayers())
                CJoinFailedPacket(RF_JFR_SERVER_IS_FULL).Send(this, Addr);
            else if(m_LoadingLevelTimestamp)
                CJoinFailedPacket(RF_JFR_LEVEL_CHANGING).Send(this, Addr);
            else if(m_pConf->GetPassword()[0] && strcmp(JoinReqPacket.GetPassword(), m_pConf->GetPassword()) != 0)
                CJoinFailedPacket(RF_JFR_INVALID_PASSWORD).Send(this, Addr);
            else if(m_pBanlist->IsBanned(Addr.GetAddress()))
                CJoinFailedPacket(RF_JFR_BANNED).Send(this, Addr);
            else if(JoinReqPacket.GetTablesVppChecksum() != m_TablesVppChecksum.GetValue() ||
                    JoinReqPacket.GetTablesVppSize() != m_cbTablesVpp)
                CJoinFailedPacket(RF_JFR_DATA_DOESNT_MATCH).Send(this, Addr);
            else {
                pPlayer = m_PlayersMgr.Add(JoinReqPacket.GetName(), Addr);
                
                if(m_pConf->GetGameType() != RF_DM && m_PlayersMgr.GetCount() / 2 > m_nBluePlayers)
                {
                    pPlayer->SetBlueTeam(true);
                    ++m_nBluePlayers;
                }
                
                CJoinSuccessPacket().Send(pPlayer);
                
                pPlayer->GetConnection()->FlushPackets(); // flush here because server info packet cannot be buffered
                CServerInfoPacket().Send(this, pPlayer->GetAddr());
                
                CPingPacket().Send(pPlayer); // RFSB depends on it
                
                CLogger::GetInst().PrintStatus("%s has joined to the game (%s)", pPlayer->GetName(), Addr.GetAddressString());
                
                CNewPlayerPacket NewPlayerPacket(pPlayer);
                for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
                    if(pPlayer2 != pPlayer)
                        NewPlayerPacket.Send(pPlayer2);
#if !LINUX
                MessageBeep(0xFFFFFFFF);
#endif
            }
            break;
        }
        case RF_PLAYER_LEFT:
        {
            CPlayerLeftPacket PlayerLeftPacket(this, Stream);
            
            if(PlayerLeftPacket.GetPlayer() != pPlayer)
            {
                CLogger::GetInst().PrintError("%s failed to left the game", pPlayer->GetName());
                break;
            }
            
            m_pGamemode->OnPlayerLeft(pPlayer);
            
            CLogger::GetInst().PrintStatus("%s has left the game", pPlayer->GetName());
            
            /* Send packet to other players */
            for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
                if(pPlayer2 != pPlayer)
                    PlayerLeftPacket.Send(pPlayer2);
            
            /* Remove player from game */
            if(pPlayer->IsBlueTeam())
                --m_nBluePlayers;
            m_PlayersMgr.Remove(pPlayer);
            
            break;
        }
        case RF_STATE_REQUEST:
        {
            /* Build statistics packet */
            CPlayerStatsPacket PlayersStatsPacket(m_LevelTimer.GetValue()/1000.0f, m_pConf->GetTimeLimit()*60.0f);
            for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
                PlayersStatsPacket.AddPlayer(pPlayer2);
            
            /* Send players statistics */
            PlayersStatsPacket.Send(pPlayer);
            
            /* Send alive entities */
            const set<CElement*> &Entities = m_pGame->GetLevel()->GetElementsMgr()->GetByType(OFET_ENTITY);
            for(set<CElement*>::const_iterator it = Entities.begin(); it != Entities.end(); ++it)
            {
                CEntity * const pEntity = (CEntity*)*it;
                if(pEntity->IsAlive())
                    CEntitySpawnPacket(pEntity, pEntity->GetPlayer()).Send(pPlayer);
            }
            
            /* Note: native server sends entity update packet before spawning players but it doesn't make sense
               because client doesn't know who is alive. Even sendingthis packet after spawn packet doesn't have effect.
               We send full update packet after RF_CLIENT_IN_GAME. */
            
            /* Send empty entity update packet */
            CEntityUpdatePacket().Send(pPlayer);
            
            /* Spawn all items for player */
            const set<CElement*> &Items = m_pGame->GetLevel()->GetElementsMgr()->GetByType(OFET_ITEM);
            for(set<CElement*>::const_iterator it = Items.begin(); it != Items.end(); ++it)
                CItemSpawnPacket((CItem*)*it).Send(pPlayer);
            
            /* Send clutter state */
            CClutterStatePacket().Send(pPlayer);
            
            /* Send glass state */
            CGlassStatePacket().Send(pPlayer);
            
            /* Send team scores in team games */
            m_pGamemode->OnPlayerJoin(pPlayer);
            
            /* Send state end packet */
            CStateEndPacket().Send(pPlayer);
            
            /* Set player as in game so sync packets will be sent to him */
            pPlayer->SetInGame(true);
            break;
        }
        case RF_SPAWN_REQUEST:
        {
            CEntity *pEntity = pPlayer->GetEntity();
            if(!pPlayer->IsInGame() || (pEntity && pEntity->IsAlive()))
                break;
            
            CSpawnRequestPacket SpawnRequestPacket(this, Stream);
            
            if(SpawnRequestPacket.GetPlayer() != pPlayer || !SpawnRequestPacket.GetCharacter())
            {
                CLogger::GetInst().PrintError("%s failed to spawn", pPlayer->GetName());
                break;
            }
            
            if(SpawnPlayer(pPlayer, SpawnRequestPacket.GetCharacter()) < 0)
                CMessagePacket("No free spawnpoint available! Please wait...").Send(pPlayer);
            
            /* Note: To reload after spawn delay is necesarry. It doesn't stop client
               from changing weapon if we spawn with some type of custom weapon.
            pPlayer->FlushPackets();
            CTimer::Sleep(10);
            CReloadPacket(pPlayer).Send(pPlayer); */
            break;
        }
        case RF_SUICIDE:
        {
            CEntity *pEntity = pPlayer->GetEntity();
            if(pEntity && pEntity->IsAlive())
            {
                SDamageInfo DmgInfo;
                DmgInfo.pResponsibleObj = pEntity;
                pEntity->Kill(DmgInfo);
            }
            break;
        }
        case RF_NAME_CHANGE:
        {
            CNameChangePacket NameChangePacket(this, Stream);
            
            if(NameChangePacket.GetPlayer() != pPlayer)
            {
                CLogger::GetInst().PrintError("%s failed to change name", pPlayer->GetName());
                break;
            }
            
            CLogger::GetInst().PrintStatus("%s is now known as %s", pPlayer->GetName(), NameChangePacket.GetName());
            pPlayer->SetName(NameChangePacket.GetName());
            
            for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
                if(pPlayer2 != pPlayer)
                    NameChangePacket.Send(pPlayer2);
            break;
        }
        case RF_RELOAD_REQUEST:
        {
            CEntity * const pEntity = pPlayer->GetEntity();
            
            if(!pEntity || !pEntity->IsAlive())
                break;
            
            if(pEntity->ReloadWeapon(NULL, false))
            {
                CReloadPacket ReloadPacket(pPlayer);
                for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
                    ReloadPacket.Send(pPlayer2);
            }
            else
                m_pGame->GetConsole()->DbgPrint("%s failed to reload %s!\n", pPlayer->GetName(), pEntity->GetWeapon()->GetClass()->strName.c_str());
            break;
        }
        
        case RF_ENTITY_UPDATE:
            CEntityUpdatePacket::Process(Stream, pPlayer);
            break;
        
        case RF_SHOOT:
        {
            CEntity * const pEntity = pPlayer->GetEntity();
            
            if(!pEntity || !pEntity->IsAlive())
                break;
            
            CShootPacket ShootPacket(Stream, pPlayer);
            
            if(ShootPacket.GetWeaponClass() != pEntity->GetWeapon()->GetClass())
            {
                CLogger::GetInst().PrintError("Invalid weapon: %s != %s",
                                    ShootPacket.GetWeaponClass() ? ShootPacket.GetWeaponClass()->strName.c_str() : "NULL",
                                    pEntity->GetWeapon()->GetClass()->strName.c_str());
                break;
            }
            
            bool bRet;
            if(ShootPacket.HasPosDir())
                bRet = pEntity->Shoot(ShootPacket.GetPos(), ShootPacket.GetDir(), ShootPacket.IsAltFire(), false);
            else
                bRet = pEntity->Shoot(ShootPacket.IsAltFire(), false);
            
            if(bRet)
            {
                for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
                    if(pPlayer2 != pPlayer)
                        ShootPacket.Send(pPlayer2);
            } else
                m_pGame->GetConsole()->DbgPrint("%s failed to shoot from %s (%u)!\n",
                                             pPlayer->GetName(), pEntity->GetWeapon()->GetClass()->strName.c_str(),
                                             pEntity->GetWeapon()->GetAmmo());
            break;
        }
        case RF_USE_REQUEST:
        {
            CEntity *pEntity = pPlayer->GetEntity();
            
            if(!pEntity || !pEntity->IsAlive())
                break;
            
            vector<CObject*> Objects = m_pGame->GetLevel()->GetColWorld()->TestPoint(pEntity->GetPos());
            for(unsigned i = 0; i < Objects.size(); ++i)
            {
                if(Objects[i]->GetType() == OFET_TRIGGER)
                {
                    CTrigger * const pTrigger = (CTrigger*)Objects[i];
                    
                    if(pTrigger->IsEnabled() && pTrigger->IsUseKeyRequired() && pTrigger->Activate(pEntity))
                    {
                        CTriggerActivatePacket TriggerActivatePacket(pTrigger, pEntity);
                        for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
                            TriggerActivatePacket.Send(pPlayer);
                    }
                }
            }
            break;
        }
        case RF_FALL_DAMAGE:
        {
            CEntity *pEntity = pPlayer->GetEntity();
            if(m_pConf->GetFallDamage() && pEntity && pEntity->IsAlive() && !pEntity->IsInvulnerabilityEnabled())
            {
                CFallDamagePacket FallDmgPacket(Stream);
                
                float fDmg = FallDmgPacket.GetForce() * 0.4f;
                
                SDamageInfo DmgInfo;
                DmgInfo.Flags = DMG_FALL;
                
                pEntity->Damage(fDmg, DmgInfo);
            }
            break;
        }
        case RF_RCON_REQUEST:
        {
            CRconRequestPacket RconReqPacket(Stream);
            
            if(m_pConf->GetRconPassword()[0] &&
               !strcmp(RconReqPacket.GetPassword(), m_pConf->GetRconPassword()))
            {
                pPlayer->SetAdmin(true);
                SendGlobalMessage(m_pGame->GetStringsTbl()->GetFormated(909, pPlayer->GetName()));
            }
            break;
        }
        case RF_RCON_COMMAND:
            if(pPlayer->IsAdmin())
            {
                CRconCommandPacket RconCmdPacket(Stream);
                const char *pCmd = RconCmdPacket.GetCommand();
                const char *pArg = strchr(pCmd, ' ');
                
                if(pArg)
                {
                    string strCmdName(pCmd, pArg - pCmd);
                    m_pGame->GetConsole()->ExecuteCommand(strCmdName.c_str(), pArg + 1, pPlayer);
                }
                else
                    m_pGame->GetConsole()->ExecuteCommand(pCmd, "", pPlayer);
            }
            break;
        
        case RF_MESSAGE:
        {
            CMessagePacket MsgPacket(this, Stream);
            if(MsgPacket.GetPlayer() == pPlayer)
            {
                for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
                    if(pPlayer2 != pPlayer && (!MsgPacket.IsTeamMsg() || pPlayer2->IsBlueTeam() == pPlayer->IsBlueTeam()))
                        MsgPacket.Send(pPlayer2);
                
                if(!MsgPacket.IsTeamMsg())
                    CLogger::GetInst().PrintStatus("%s: %s", pPlayer->GetName(), MsgPacket.GetMessage());
            }
            else
                CLogger::GetInst().PrintError("%s failed to send message", pPlayer->GetName());
            break;
        }
        
        case RF_PONG:
            pPlayer->StopPingTimer();
            break;
        
        case RF_TEAM_CHANGE:
        {
            if(m_pConf->GetGameType() == RF_DM)
                break;
            
            CTeamChangePacket TeamChangePacket(this, Stream);
            
            if(TeamChangePacket.GetPlayer() != pPlayer)
            {
                CLogger::GetInst().PrintError("%s failed to change team.", pPlayer->GetName());
                break;
            }
            
            if(pPlayer->IsBlueTeam())
                --m_nBluePlayers;
            pPlayer->SetBlueTeam(TeamChangePacket.IsBlueTeam());
            if(pPlayer->IsBlueTeam())
                ++m_nBluePlayers;
            CLogger::GetInst().PrintError("%s changed team to %s.", pPlayer->GetName(), pPlayer->IsBlueTeam() ? "blue" : "red");
            
            for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
                TeamChangePacket.Send(pPlayer2);
            
            /* Note: Native client sends suicide packets after team change but we should not rely on it */
            CEntity *pEntity = pPlayer->GetEntity();
            if(pEntity)
            {
                SDamageInfo DmgInfo;
                DmgInfo.pResponsibleObj = pEntity;
                pEntity->Kill(DmgInfo);
            }
            break;
        }
        
        case RF_CLIENT_IN_GAME:
        {
            /* Synchronize all entities */
            CEntityUpdatePacket EntityUpdatePacket;
            const set<CElement*> &Entities = m_pGame->GetLevel()->GetElementsMgr()->GetByType(OFET_ENTITY);
            for(set<CElement*>::const_iterator it = Entities.begin(); it != Entities.end(); ++it)
                EntityUpdatePacket.Add((CEntity*)*it);
            
            EntityUpdatePacket.Send(pPlayer, true);
            break;
        }
        
        case RF_PLAYER_RATE_CHANGE:
            /* Ignore */
            break;
        
        default:
            m_pGame->GetConsole()->DbgPrint("Unknown game packet: 0x%x:", nType);
            for(unsigned i = 0; i < cBytes; ++i)
                m_pGame->GetConsole()->DbgPrint(" %02X", Stream.ReadUInt8());
            m_pGame->GetConsole()->DbgPrint("\n");
            break;
    }
}

void CServer::ProcessGamePackets(CInputBinaryStream &Stream, const CSocketAddress &Addr)
{
    while(Stream.good())
    {
        unsigned nType = Stream.ReadUInt8();
        unsigned nSize = Stream.ReadUInt16();
        if(!Stream.good())
            break;
        streampos nPos = Stream.tellg();
        ProcessGamePacket(nType, Stream, Addr, nSize);
        Stream.seekg(nPos + (streampos)nSize);
    }
}

void CServer::ProcessSyncPacket(CInputBinaryStream &Stream, const CSocketAddress &Addr)
{
    CPlayer *pPlayer = m_PlayersMgr.Get(Addr);
    if(!pPlayer)
        return;
    
    rfReliable Packet;
    Stream.ReadBinary(&Packet, sizeof(Packet));
    
    pPlayer->m_Timestamp = time(NULL);
    
    switch(Packet.type)
    {
        case RF_REL_REPLY: /* reply */
        {
            unsigned nPacketId = Stream.ReadUInt16();
            pPlayer->GetConnection()->AcceptReliablePacket(nPacketId);
            break;
        }
        
        case RF_REL_PACKETS:
        {
            CReplyPacket(Packet.id, Packet.ticks).Send(pPlayer);
            ProcessGamePackets(Stream, Addr);
            break;
        }
        
        case RF_REL_JOIN_05:
            pPlayer->GetConnection()->AddSpecialReliablePacket(RF_REL_JOIN_05);
            /* unknown */
            break;
        
        case RF_REL_JOIN_03:
            CReplyPacket(Packet.id, Packet.ticks).Send(pPlayer);
            break;
        
        case RF_REL_JOIN_06:
        {
            CReplyPacket(Packet.id, Packet.ticks).Send(pPlayer);
            /* FIXME: what is this packet for? */
            pPlayer->GetConnection()->AddSpecialReliablePacket(RF_REL_JOIN_05);
            
            /* Build and send players list packet */
            CPlayersListPacket PlayersListPacket;
            for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL, true); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2, true))
                if(pPlayer2 != pPlayer)
                    PlayersListPacket.AddPlayer(pPlayer2);
            PlayersListPacket.Send(pPlayer);
            break;
        }
        
        default:
            CLogger::GetInst().PrintStatus("Unknown reliable packet: 0x%02x", Packet.type);
    }
}

DECLARE_THREAD_FUNC(CServer::RecvThreadProcStatic, param)
{
    CServer *pServer = (CServer*)param;
    pServer->RecvThreadProc();
    return 0;
}

void CServer::RecvThreadProc()
{
    char Buf[1024];
    unsigned cPackets = 0, bDDosWarning = 0;
    time_t Timestamp = 0;
    CSocketAddress Addr;
    
    for(;;)
    {
        int cbBuf = m_Sock.RecvFrom(Buf, sizeof(Buf), Addr);
        if(cbBuf < 0 && CSocket::GetLastError() != 10054)
            CLogger::GetInst().PrintError("Error %u! recvfrom failed.", CSocket::GetLastError());
        
        /* Ignore too short packets */
        if(cbBuf < 4)
            continue;
        
        if(time(NULL) - Timestamp >= 60)
        {
            Timestamp = time(NULL);
            cPackets = 0;
            bDDosWarning = 0;
        } else
            ++cPackets;
        
        if(cPackets > MAX_PACKETS_PER_MIN)
        {
            if(!bDDosWarning)
            {
                bDDosWarning = 1;
                CLogger::GetInst().PrintError("Warning! Limit of recived packets per minute (%u) has been reached.", MAX_PACKETS_PER_MIN);
            }
            continue;
        }
        
        g_Lock.Acquire();
        
        CInputBinaryStringStream Stream(string(Buf, cbBuf));
        /* First byte says what type of packet is it */
        switch(Stream.ReadUInt8())
        {
            case RF_GAME: /* Game packet */
                ProcessGamePackets(Stream, Addr);
                break;
            
            case RF_RELIABLE: /* Reliable packet */
                ProcessSyncPacket(Stream, Addr);
                break;
            
            case RF_TRACKER: /* Tracker packet */
            {
                static bool bTrackerResponded = false;
                if(!bTrackerResponded)
                {
                    CLogger::GetInst().PrintStatus("Server has been successfully added to tracker list!");
                    bTrackerResponded = true;
                }
                break;
            }
            
            default:
                CLogger::GetInst().PrintError("Warning! Unknown packet main type: 0x%02X", Buf[0]);
        }
        
        FlushPackets();
        
        g_Lock.Release();
    }
}

void CServer::PingTracker()
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(RF_TRACKER);
    Stream.WriteUInt8(0x06);                    // unknown
    Stream.WriteUInt16(RF_TRACKER_SERVER_PING); // type
    Stream.WriteUInt32(0);                      // seq
    Stream.WriteUInt16(0x000A);                 // packet_len
    Stream.WriteUInt8(0x00);                    // unknown2
    
    CSocketAddress Addr(RF_TRACKER_HOST, RF_TRACKER_PORT);
    m_Sock.SendTo(Stream.str(), Addr);
}

void CServer::PingPlayers()
{
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        if(pPlayer->IsInGame())
        {
            CPingPacket().Send(pPlayer);
            pPlayer->StartPingTimer();
        }
}

void CServer::KickPlayer(CPlayer *pPlayer, rfLeftReason nReason, bool bTimedOut)
{
    /* Send player left packet */
    CPlayerLeftPacket PlayerLeftPacket(pPlayer, nReason);
    for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
        if(!bTimedOut || pPlayer2 != pPlayer)
            PlayerLeftPacket.Send(pPlayer2);
    
    m_pGamemode->OnPlayerLeft(pPlayer);
    
    /* Synchronize blue players counter */
    if(pPlayer->IsBlueTeam())
        --m_nBluePlayers;
    
    if(bTimedOut)
        CLogger::GetInst().PrintStatus("Player %s is timed out", pPlayer->GetName());
    else
        CLogger::GetInst().PrintStatus("%s has been kicked", pPlayer->GetName());
    
    /* Remove player object */
    m_PlayersMgr.Remove(pPlayer);
}

void CServer::KickTimedOutPlayers()
{
    CPlayer *pPlayer = m_PlayersMgr.Enum(NULL);
    while(pPlayer)
    {
        CPlayer *pNext = m_PlayersMgr.Enum(pPlayer);
        
        /* Check for timeout */
        if(pPlayer->GetAddr().GetAddress() != 0 && time(NULL) - pPlayer->m_Timestamp > (time_t)TIMEOUT_VALUE)
            KickPlayer(pPlayer, RF_LR_KICKED, true);
        
        pPlayer = pNext;
    }
}

int CServer::SpawnPlayer(CPlayer *pPlayer, const SMpCharacter *pCharacter, btVector3 *pPos)
{
    CEntity *pEntity;
    if(pPos)
        pEntity = pPlayer->Spawn(pCharacter, *pPos);
    else
    {
        const CSpawnpoint *pSpawnpoint = m_pGame->GetLevel()->GetSpawnpointsMgr()->Find(m_pConf->GetGameType() != RF_DM, pPlayer->IsBlueTeam());
        if(!pSpawnpoint)
            return -1;
        
        pEntity = pPlayer->Spawn(pCharacter, pSpawnpoint->GetPos(), pSpawnpoint->GetRotMatrix());
    }
    
    pEntity->RemoveWeapons();
	pEntity->AddWeapon(m_pGame->GetWeaponsTbl()->Get(RF_PISTOL), 64);
	pEntity->AddWeapon(m_pGame->GetWeaponsTbl()->Get(RF_CONTROL_BATON), 200);
	const char *pszDefaultWeapon = m_pConf->GetDefaultWeapon();
	if(!pszDefaultWeapon || !pszDefaultWeapon[0])
        pszDefaultWeapon = m_pGame->GetGameTbl()->GetDefaultWeapon();
	const SWeaponClass *pWeaponCls = m_pGame->GetWeaponsTbl()->Get(pszDefaultWeapon);
	if(!pWeaponCls)
        CLogger::GetInst().PrintError("Invalid default weapon: %s", pszDefaultWeapon ? pszDefaultWeapon : "");
	
	if(pWeaponCls)
	{
	    if(pWeaponCls->nId != RF_PISTOL && pWeaponCls->nId != RF_CONTROL_BATON)
            pEntity->AddWeapon(pWeaponCls, pWeaponCls->nClipSize);
        
        pEntity->SwitchWeapon(pWeaponCls);
	}
	else
        pEntity->SwitchWeapon(m_pGame->GetWeaponsTbl()->Get(RF_PISTOL));
    
    CEntitySpawnPacket EntitySpawnPacket(pPlayer->GetEntity(), pPlayer);
    for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
        if(pPlayer2->IsInGame())
            EntitySpawnPacket.Send(pPlayer2);
    
    return 0;
}

void CServer::SendUpdateItemsPacket()
{
    CItemsUpdatePacket ItemsUpdatePacket;
    
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        if(pPlayer->IsInGame())
            ItemsUpdatePacket.Send(pPlayer);
}

void CServer::SendUpdateEntitiesPacket()
{
    /* Build entity update packet for all alive players */
    CEntityUpdatePacket EntityUpdatePacket;
    
    /* Add entities if level is fully loaded */
    if(!m_LoadingLevelTimestamp)
    {
        // Update packet doesn't work for entites without players.
        //for(unsigned i = 0; i < m_pLevel->GetEntities().size(); ++i)
        //    EntityUpdatePacket.Add(m_pLevel->GetEntities()[i]);
        for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        {
            CEntity *pEntity = pPlayer->GetEntity();
            if(pEntity)
                EntityUpdatePacket.Add(pEntity);
        }
            
    }
    
    /* Send packet */
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        if(pPlayer->IsInGame())
            EntityUpdatePacket.Send(pPlayer);
    
#if LOW_BANDWIDTH_USAGE
    /* Clean "changed" state */
    const set<CElement*> &Entities = m_pGame->GetLevel()->GetElementsMgr()->GetByType(OFET_ENTITY);
    for(set<CElement*>::const_iterator it = Entities.begin(); it != Entities.end(); ++it)
    {
        CEntity * const pEntity = (CEntity*)*it;
        pEntity->SetPosRotAnimChanged(false);
        pEntity->SetLifeArmorChanged(false);
        pEntity->SetWeaponChanged(false);
        pEntity->SetArmorStateChanged(false);
    }
#endif
}

void CServer::SendStatsPacket()
{
    /* Build statistics packet */
    CPlayerStatsPacket PlayersStatsPacket(m_LevelTimer.GetValue()/1000.0f, m_pConf->GetTimeLimit()*60.0f);
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        PlayersStatsPacket.AddPlayer(pPlayer);
        
    /* Send to players */
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        if(pPlayer->IsInGame())
            PlayersStatsPacket.Send(pPlayer);
}

void CServer::FinishLevelChange()
{
    /* Send level change packet */
    CLevelChangePacket LvlChangePacket(m_strLevelFileName.c_str(), m_pGame->GetLevel()->GetChecksum());
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
    {
        LvlChangePacket.Send(pPlayer);
        pPlayer->SetScore(0);
        pPlayer->SetCaptures(0);
    }
    
#if CREATE_PLAYERS_FOR_NPCS
    /* Kick bots from previous level */
    CPlayer *pNext;
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL, true); pPlayer; pPlayer = pNext)
    {
        pNext = m_PlayersMgr.Enum(pPlayer, true);
        if(pPlayer->GetAddr().GetAddress() == 0)
            KickPlayer(pPlayer, RF_LR_NORMAL);
    }
    
    const set<CElement*> &Entities = m_Level.GetElements(OFET_ENTITY);
    for(set<CElement*>::const_iterator it = Entities.begin(); it != Entities.end(); ++it)
    {
        CEntity * const pEntity = (CEntity*)*it;
        if(pEntity->IsAlive())
        {
            /* Don't create bots if server is full */
            if(m_PlayersMgr.GetCount() >= m_Conf.GetMaxPlayers())
                break;
            
            /* Create bot */
            stringstream NameStream;
            NameStream << string("Bot") << i;
            CPlayer *pBot = m_PlayersMgr.Add(NameStream.str().c_str(), CSocketAddress());
            
            if(pBot)
            {
                /* Initialize bot player */
                pBot->SetScore(-100);
                pEntity->SetPlayer(pBot);
                pBot->SetEntity(pEntity);
                
                /* Send new player packet */
                CNewPlayerPacket NewPlayerPacket(pBot);
                for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
                    NewPlayerPacket.Send(pPlayer);
            }
        }
    }
#endif
    
    // Reset timers
    m_LoadingLevelTimestamp = 0;
    m_LevelTimer.Reset();
    
    // Reset gamemode
    delete m_pGamemode;
    m_pGamemode = CGamemode::Create(this, m_pConf->GetGameType());
    
    // Display message
    CLogger::GetInst().PrintStatus("Loaded level: %s (%s)", m_pGame->GetLevel()->GetName(), m_strLevelFileName.c_str());
}

void CServer::FlushPackets()
{
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        pPlayer->GetConnection()->FlushPackets();
}

DECLARE_THREAD_FUNC(CServer::PingThreadProcStatic, param)
{
    CServer *pServer = (CServer*)param;
    pServer->PingThreadProc();
    return 0;
}

void CServer::PingThreadProc()
{
    CTimer PingPlayersTimer;
    CTimer ItemsUpdateTimer;
    CTimer StatsTimer;
    CTimer TrackerPingTimer;
    CTimer DeltaTimer;
    
    PingTracker(); /* Add server to tracker */
    
    while(true)
    {
        CTimer::Sleep(1000 / m_pConf->GetNetFps());
        
        unsigned nDeltaTime = DeltaTimer.GetValue();
        DeltaTimer.Reset();
        
        g_Lock.Acquire();
        
        /* Ping tracker */
        if(TrackerPingTimer.GetValue() >= TRACKER_PING_INTERVAL)
        {
            PingTracker();
            TrackerPingTimer.Reset();
        }
        
        /* Kick players which lost connection */
        KickTimedOutPlayers();
        
        /* Load queued level change if needef */
        if(!m_strQueuedLevelChange.empty())
        {
            LoadLevel(m_strQueuedLevelChange.c_str());
            m_strQueuedLevelChange.resize(0);
        }
        
        /* Check if level should end now */
        if(m_LoadingLevelTimestamp == 0 && m_LevelTimer.GetValue() >= m_pConf->GetTimeLimit() * 60 * 1000)
        {
            try
            {
                LoadNextLevel();
            }
            catch(const CException &e)
            {
                CLogger::GetInst().PrintError("Warning! Failed to load next level. %s", e.what());
            }
        }
        
        /* Change level after some delay */
        if(m_LoadingLevelTimestamp != 0 && (time(NULL) - m_LoadingLevelTimestamp > LOADING_LEVEL_DELAY || m_PlayersMgr.GetCount() == 0))
            FinishLevelChange();
        
        /* Return flags if they are droped to long */
        m_pGamemode->Update(nDeltaTime);
        
        /* Update level */
        m_pGame->GetLevel()->Update(nDeltaTime);
        
#if CREATE_PLAYERS_FOR_NPCS
        /* Update bots */
        for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL, true); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer, true))
        {
            CEntity *pEntity = pPlayer->GetEntity()
            if(pPlayer->GetAddr().GetAddress() == 0 && pEntity && pEntity->IsAlive())
                pEntity->SetPos(pEntity->GetPos() + btVector3(((rand() % 11) - 5) / 10.0f, 0.0f, ((rand() % 11) - 5) / 10.0f));
        }
#endif /* CREATE_PLAYERS_FOR_NPCS */
        
        /* Ping players */
        if(PingPlayersTimer.GetValue() >= PLAYER_PING_INTERVAL)
        {
            PingPlayers();
            PingPlayersTimer.Reset();
        }
        
        /* Send game statistics */
        if(StatsTimer.GetValue() >= STATS_UPDATE_INTERVAL)
        {
            SendStatsPacket();
            StatsTimer.Reset();
        }
        
        /* Send entities update packet */
        SendUpdateEntitiesPacket();
        
        /* Send item update packet */
        if(ItemsUpdateTimer.GetValue() >= ITEMS_UPDATE_INTERVAL)
        {
            SendUpdateItemsPacket();
            ItemsUpdateTimer.Reset();
        }
        
        /* Flush buffered packets */
        FlushPackets();
        
        g_Lock.Release();
    }
}

void CServer::OnTiggerActivate(CTrigger *pTrigger, CEntity *pEntity)
{
    CTriggerActivatePacket TriggerActivatePacket(pTrigger, pEntity);
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        TriggerActivatePacket.Send(pPlayer);
}

bool CServer::OnItemApply(CItem *pItem, CEntity *pEntity)
{
    // Check if gamemode allows picking this item up
    if(!m_pGamemode->OnItemApply(pItem, pEntity))
        return true;
    
    // Process the event
    pItem->Apply(pEntity);
    
    // Send packet to player who picked the item up
    CItemApplyPacket ItemApplyPacket(pItem, pEntity);
    if(pEntity->GetPlayer())
        ItemApplyPacket.Send(pEntity->GetPlayer());
    
    if(!pItem->GetClass()->bWeapon || !m_pConf->GetWeaponsStay() || pItem->GetBit() == 0xFFFF)
    {
        // Remove the item
        if(pItem->GetBit() >= 0xFFFF)
        {
            /* This is not bitmap item so we have to send apply packet to all players */
            for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
                if(pPlayer != pEntity->GetPlayer())
                    ItemApplyPacket.Send(pPlayer);
        }
        
        pItem->SetEnabled(false);
    }
    
    return true;
}

void CServer::OnClutterKill(CClutter *pClutter, SDamageInfo &DmgInfo)
{
    /* Send clutter kill packet */
    CClutterKillPacket ClutterKillPacket(pClutter);
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        ClutterKillPacket.Send(pPlayer);
}

void CServer::OnEntityKill(CEntity *pEntity, SDamageInfo &DmgInfo)
{
    CEntity *pKiller = DmgInfo.pResponsibleObj && DmgInfo.pResponsibleObj->GetType() == OFET_ENTITY ? (CEntity*)DmgInfo.pResponsibleObj : NULL;
    CWeapon *pWeapon = pEntity->GetWeapon();
    CItem *pItem = NULL;
    if(pWeapon->GetAmmo() || pEntity->GetAmmo())
    {
        const SItemClass *pItemClass = m_pGame->GetItemsTbl()->FindWeapon(pWeapon->GetClass());
        if(pItemClass)
        {
            CItem *pItem = new CItem(m_pGame->GetLevel(), pItemClass);
            pItem->SetPos(pEntity->GetPos());
            pItem->SetCount(pWeapon->GetAmmo() + pEntity->GetAmmo());
        }
    }
    
    /* Send kill packet */
    CEntityKillPacket EntityKillPacket(pEntity, pEntity->GetPlayer(), pKiller ? pKiller->GetPlayer() : NULL, pItem);
    for(CPlayer *pPlayer2 = m_PlayersMgr.Enum(NULL); pPlayer2; pPlayer2 = m_PlayersMgr.Enum(pPlayer2))
        EntityKillPacket.Send(pPlayer2);
    
    m_pGamemode->OnEntityKill(pEntity, DmgInfo);
}

void CServer::OnGlassKill(CRoom *pRoom, SDamageInfo &DmgInfo)
{
    /* Send clutter kill packet */
    bool bExplosion = (DmgInfo.Flags & DMG_EXPLOSION) ? true : false;
    CGlassKillPacket GlassKillPacket(pRoom, bExplosion, DmgInfo.vPos);
    for(CPlayer *pPlayer = m_PlayersMgr.Enum(NULL); pPlayer; pPlayer = m_PlayersMgr.Enum(pPlayer))
        GlassKillPacket.Send(pPlayer);
}
