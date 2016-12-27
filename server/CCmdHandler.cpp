/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CCmdHandler.cpp
*  PURPOSE:     Command handlers for server console
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CCmdHandler.h"
#include "CServer.h"
#include "CPlayer.h"
#include "CConsole.h"
#include "CServerConfig.h"
#include "CBanlist.h"
#include "CLevel.h"
#include "CVirtualFileSystem.h"
#include "util/CException.h"
#include "util/utils.h"
#include "version.h"
#include "main.h"
#include "util/CLock.h"
#include "tables/CWeaponsTable.h"

using namespace std;

bool CCmdHandler::HandleCmd(const char *pCommand, const char *pArg, CConsole::TUser hUser)
{
    CPlayer *pCaller = (CPlayer*)hUser;
    
    g_Lock.Acquire();
    
    /* Parse command */
    if(!StrCmpI(pCommand, "quit") || !StrCmpI(pCommand, "exit"))
        return true;
    else if(!StrCmpI(pCommand, "help") || !StrCmpI(pCommand, "h"))
        m_pConsole->CmdOutput(pCaller, "Commands list:\n"
                  "help                     - displays this text\n"
                  "load_config name         - loads configuration from dedicated_servers.txt\n"
                  "log_file [path]          - informs if logging to file is enabled and optionaly\n"
                  "                           enables logging to specified file. To disable logging\n"
                  "                           use false as argument\n"
                  "ban player_name          - bans specified player\n"
                  "ban_ip ip/ip_range       - bans IP or IP range\n"
                  "unban ip/ip range        - removes IP or IP range from banlist (does not remove\n"
                  "                           part of range)\n"
                  "unban_last               - unbans last added IP or IP range in current server\n"
                  "                           session\n"
                  "info                     - displays information about netgame\n"
                  "kick player_name         - kicks specified player\n"
                  "kick_all                 - kicks all players from the server except RCON\n"
                  "                           handlers\n"
                  "level [rfl_file_name]    - displays current level and optionally changes level\n"
                  "level_next               - loads next level\n"
                  "level_restart            - restarts current level\n"
                  "port                     - displays server port\n"
                  "password [pw]            - displays server password and optionally changes it. To\n"
                  "                           remove password use false as argument\n"
                  "quit                     - closes server\n"
                  "reload_banlist           - reloads banlist from banlist.txt\n"
                  "say message              - sends message to clients\n"
                  "weapon                   - displays weapon of spawning players and optionally\n"
                  "                           changes it\n"
                  "version                  - displays server version");
    else if(!StrCmpI(pCommand, "load_config"))
    {
        try
        {
            m_pServer->GetConf()->Load(pArg);
        }
        catch(std::exception &e)
        {
            m_pConsole->CmdOutput(pCaller, "Failed to load configuration. %s", e.what());
        }
    }
    else if(!StrCmpI(pCommand, "log_file"))
    {
        if(!strcmp(pArg, "false"))
            m_pServer->GetConf()->SetLogFile("");
        else if(pArg[0])
            m_pServer->GetConf()->SetLogFile(pArg);
        
        if(m_pServer->GetConf()->GetLogFile()[0])
            m_pConsole->CmdOutput(pCaller, "Logging to file %s is enabled.", m_pServer->GetConf()->GetLogFile());
        else
            m_pConsole->CmdOutput(pCaller, "Logging to file is disabled.");
    }
    else if(!StrCmpI(pCommand, "reload_banlist"))
    {
        if(m_pServer->GetBanlist()->Load() == 0)
            m_pConsole->CmdOutput(pCaller, "Banlist has been reloaded.");
        else
            m_pConsole->CmdOutput(pCaller, "Cannot reload banlist!");
    }
    else if(!StrCmpI(pCommand, "ban"))
    {
        CPlayer *pPlayer = m_pServer->GetPlayersMgr()->FindPlayer(pArg);
        
        if(pPlayer)
        {
            m_strLastBan = pPlayer->GetAddr().GetAddressString();
            m_pConsole->CmdOutput(pCaller, "%s has been banned.", pPlayer->GetName());
            m_pServer->GetBanlist()->Ban(pPlayer->GetAddr().GetAddress());
        } else
            m_pConsole->CmdOutput(pCaller, "Cannot find player \"%s\"!", pArg);
    }
    else if(!StrCmpI(pCommand, "ban_ip"))
    {
        if(m_pServer->GetBanlist()->Ban(pArg) >= 0)
        {
            m_strLastBan = pArg;
            m_pConsole->CmdOutput(pCaller, "%s has been banned.", pArg);
        }
        else
            m_pConsole->CmdOutput(pCaller, "Cannot ban %s!", pArg);
    }
    else if(!StrCmpI(pCommand, "unban"))
    {
        if(m_pServer->GetBanlist()->Unban(pArg) == 0)
            m_pConsole->CmdOutput(pCaller, "%s has been unbanned.", pArg);
        else
            m_pConsole->CmdOutput(pCaller, "Cannot unban \"%s\"!", pArg);
    }
    else if(!StrCmpI(pCommand, "unban_last"))
    {
        if(!m_strLastBan.empty())
        {
            m_pServer->GetBanlist()->Unban(m_strLastBan.c_str());
            m_pConsole->CmdOutput(pCaller, "%s has been unbanned.", m_strLastBan.c_str());
            m_strLastBan.clear();
        }
        else
            m_pConsole->CmdOutput(pCaller, "No bans added recently!");
    }
    else if(!StrCmpI(pCommand, "info"))
    {
        m_pConsole->CmdOutput(pCaller, "Current level: %s (%s)", m_pServer->GetLevel()->GetName(), m_pServer->GetLevelFileName());
        m_pConsole->CmdOutput(pCaller, "Level time: %u:%02u:%02u",
                  m_pServer->GetLevelTime() / 1000 / 3600,
                  (m_pServer->GetLevelTime() / 1000 / 60) % 60,
                  (m_pServer->GetLevelTime() / 1000) % 60);
        
        CPlayer *pPlayer = m_pServer->GetPlayersMgr()->Enum(NULL);
        if(pPlayer)
        {
            m_pConsole->CmdOutput(pCaller, "Players list:");
            while(pPlayer)
            {
                m_pConsole->CmdOutput(pCaller, "%s (%s)", pPlayer->GetName(), pPlayer->GetAddr().GetAddressString());
                pPlayer = m_pServer->GetPlayersMgr()->Enum(pPlayer);
            }
        }
    }
    else if(!StrCmpI(pCommand, "kick"))
    {
        CPlayer *pPlayer = m_pServer->GetPlayersMgr()->FindPlayer(pArg);
        
        if(pPlayer)
            m_pServer->KickPlayer(pPlayer);
        else
            m_pConsole->CmdOutput(pCaller, "Cannot find player \"%s\"!", pArg);
    }
    else if(!StrCmpI(pCommand, "kick_all"))
    {
        for(CPlayer *pPlayer = m_pServer->GetPlayersMgr()->Enum(NULL); pPlayer; pPlayer = m_pServer->GetPlayersMgr()->Enum(pPlayer))
            if(!pPlayer->IsAdmin())
                m_pServer->KickPlayer(pPlayer);
    }
    else if(!StrCmpI(pCommand, "level"))
    {
        if(pArg[0])
        {
            CString strLevel = pArg;
            strLevel += ".rfl";
            try
            {
                m_pServer->LoadLevel(strLevel.c_str());
            }
            catch(const CException &e)
            {
                vector<CString> Files = CVirtualFileSystem::GetInst().FindFiles(pArg, ".rfl");

                if (Files.size() > 1)
                {
                    m_pConsole->CmdOutput(pCaller, "More than one level found:");
                    for (unsigned i = 0; i < Files.size(); ++i)
                        m_pConsole->CmdOutput(pCaller, "%s", Files[i].c_str());
                }
                else if (Files.size() == 1)
                {
                    if (Files[0] == strLevel)
                        m_pConsole->CmdOutput(pCaller, "Failed to load level \"%s\"! %s", Files[0].c_str(), e.what());
                    else
                    {
                        try
                        {
                            // Try to load level found
                            m_pServer->LoadLevel(Files[0].c_str());
                        }
                        catch (const CException &e2)
                        {
                            m_pConsole->CmdOutput(pCaller, "Failed to load level \"%s\"! %s", Files[0].c_str(), e2.what());
                        }
                    }
                }
                else
                    m_pConsole->CmdOutput(pCaller, "Cannot find level \"%s\"!", pArg);
            }
        } else
            m_pConsole->CmdOutput(pCaller, "Current level: %s (%s)", m_pServer->GetLevel()->GetName(), m_pServer->GetLevelFileName());
    }
    else if(!StrCmpI(pCommand, "level_next"))
    {
        try
        {
            m_pServer->LoadNextLevel();
        }
        catch(const CException &e)
        {
            m_pConsole->CmdOutput(pCaller, "Failed to load next level! %s", e.what());
        }
    }
    else if(!StrCmpI(pCommand, "level_restart"))
    {
        CString strLevelNameCopy = m_pServer->GetLevelFileName();
        try
        {
            m_pServer->LoadLevel(strLevelNameCopy.c_str());
        }
        catch(const CException &e)
        {
            m_pConsole->CmdOutput(pCaller, "Failed to restart level! %s", e.what());
        }
    }
    else if(!StrCmpI(pCommand, "name"))
    {
        if(pArg[0])
            m_pServer->GetConf()->SetName(pArg);
        
        m_pConsole->CmdOutput(pCaller, "Server name: %s", m_pServer->GetConf()->GetName());
    }
    else if(!StrCmpI(pCommand, "netfps"))
    {
        if(pArg[0])
            m_pServer->GetConf()->SetNetFps(strtoul(pArg, NULL, 0));
        
        m_pConsole->CmdOutput(pCaller, "Server net FPS: %u", m_pServer->GetConf()->GetNetFps());
    }
    else if(!StrCmpI(pCommand, "password"))
    {
        if(pArg[0])
        {
            if(!strcmp(pArg, "false"))
                m_pServer->GetConf()->SetPassword("");
            else
                m_pServer->GetConf()->SetPassword(pArg);
        }
        
        if(m_pServer->GetConf()->GetPassword()[0])
            m_pConsole->CmdOutput(pCaller, "Server password: %s", m_pServer->GetConf()->GetPassword());
        else
            m_pConsole->CmdOutput(pCaller, "Server has no password.");
    }
    else if(!StrCmpI(pCommand, "port"))
    {
        m_pConsole->CmdOutput(pCaller, "Server port: %u", m_pServer->GetConf()->GetPort());
    }
    else if(!StrCmpI(pCommand, "say"))
    {
        m_pServer->SendGlobalMessage(pArg);
    }
    else if(!StrCmpI(pCommand, "weapon"))
    {
        if(pArg[0])
        {
            const SWeaponClass *pWeaponCls = m_pServer->GetGame()->GetTables()->weapons()->Get(pArg);
            if(!pWeaponCls)
                pWeaponCls = m_pServer->GetGame()->GetTables()->weapons()->Get(strtoul(pArg, NULL, 0));
            if(pWeaponCls)
                m_pServer->GetConf()->SetDefaultWeapon(pWeaponCls->strName.c_str());
        }
        
        const char *pszWeaponCls = m_pServer->GetConf()->GetDefaultWeapon();
        m_pConsole->CmdOutput(pCaller, "Default weapon: %s", pszWeaponCls ? pszWeaponCls : "Unknown");
    }
    else if(!StrCmpI(pCommand, "ver") || !StrCmpI(pCommand, "version"))
    {
        m_pConsole->CmdOutput(pCaller, PRODUCT_NAME " " PRODUCT_VERSION);
    }
    else
        m_pConsole->CmdOutput(pCaller, "Unknown command: \"%s\"!", pCommand);
    
    g_Lock.Release();
    
    return false;
}
