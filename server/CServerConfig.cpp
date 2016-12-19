/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CServerConfig.cpp
*  PURPOSE:     Reads server configuration
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "precomp.h"
#include "utils.h"
#include "errors.h"
#include "CServerConfig.h"
#include "CLogger.h"
#include "tables/CTblReader.h"
#include "CException.h"

using namespace std;

void CServerConfig::Load(const char *pszConfName)
{
    ifstream Stream("dedicated_server.txt");
    if(Stream.fail())
        THROW_EXCEPTION("Failed to open dedicated_server.txt");
    
    /* Initialize all settings to defaults */
    /*m_strName = DEFAULT_NAME;
    m_nGameType = DEFAULT_GAME_TYPE;
    m_nMaxPlayers = DEFAULT_MAX_PLAYERS;
    m_nTimeLimit = DEFAULT_TIME_LIMIT;
    m_nKillsLimit = DEFAULT_KILLS_LIMIT;
    m_nCapturesLimit = DEFAULT_CAPTURES_LIMIT;
    m_nGeomodLimit = DEFAULT_GEOMOD_LIMIT;
    m_bTeamDamage = DEFAULT_TEAM_DAMAGE;
    m_bFallDamage = DEFAULT_FALL_DAMAGE;
    m_bWeaponStay = DEFAULT_WEAPON_STAY;
    m_bForceRespawn = DEFAULT_FORCE_RESPAWN;
    m_bBalanceTeams = DEFAULT_BALANCE_TEAMS;

    m_nPort = DEFAULT_PORT;
    m_bShowPlayerIps = DEFAULT_SHOW_PLAYER_IPS;
    CLogger::GetInst().Disable();
    CLogger::GetInst().SetPath("server.log");*/

    CTblReader Reader(Stream, "Dedicated Server Options");
    if(!Reader.IsSectionFound())
        THROW_EXCEPTION("Cannot find section \"Dedicated Server Options\"");
    
    bool bFoundConf = false;
    int iStatus = 0;
    while(Reader.LoadNextElement() == 0)
    {
        const char *pVar = Reader.GetName();
        if(pVar[0] != '$')
            continue;
        
        ++pVar;
        
        if(StrCmpI(pVar, "Name") == 0)
        {
            CString strName;
            Reader.GetString(strName);
            if(strName == pszConfName)
            {
                bFoundConf = true;
                m_Levels.clear();
            } else if(bFoundConf)
                break;
        }
        else if(bFoundConf)
        {
            if(StrCmpI(pVar, "Server Name") == 0)
                Reader.GetString(m_strName);
            else if(StrCmpI(pVar, "Game Type") == 0)
            {
                CString strGameType;
                Reader.GetString(strGameType);
                if(iStatus == 0)
                    m_nGameType = StrToGameType(strGameType.c_str());
            }
            else if(StrCmpI(pVar, "Max Players") == 0)
                Reader.GetUInt(m_nMaxPlayers);
            else if(StrCmpI(pVar, "Password") == 0)
                Reader.GetString(m_strPassword);
            else if(StrCmpI(pVar, "Rcon Password") == 0)
                Reader.GetString(m_strRconPassword);
            else if(StrCmpI(pVar, "Max Time") == 0)
                Reader.GetUInt(m_nTimeLimit);
            else if(StrCmpI(pVar, "Max Kills") == 0)
                Reader.GetUInt(m_nKillsLimit);
            else if(StrCmpI(pVar, "Max Captures") == 0)
                Reader.GetUInt(m_nCapturesLimit);
            else if(StrCmpI(pVar, "Geomod Limit") == 0)
                Reader.GetUInt(m_nGeomodLimit);
            else if(StrCmpI(pVar, "Team Damage") == 0)
                Reader.GetBool(m_bTeamDamage);
            else if(StrCmpI(pVar, "Fall Damage") == 0)
                Reader.GetBool(m_bFallDamage);
            else if(StrCmpI(pVar, "Weapons Stay") == 0)
                Reader.GetBool(m_bWeaponsStay);
            else if(StrCmpI(pVar, "Force Respawn") == 0)
                Reader.GetBool(m_bForceRespawn);
            else if(StrCmpI(pVar, "Balance Teams") == 0)
                Reader.GetBool(m_bBalanceTeams);
            else if(StrCmpI(pVar, "Map") == 0)
            {
                CString strLevel;
                Reader.GetString(strLevel);
                m_Levels.push_back(strLevel);
            }
            /* New options */
            else if(StrCmpI(pVar, "Port") == 0)
                Reader.GetUInt(m_nPort);
            else if(StrCmpI(pVar, "Mask Ip") == 0)
                Reader.GetBool(m_bMaskIp);
            else if(StrCmpI(pVar, "Spawn Weapon") == 0)
                Reader.GetString(m_strDefaultWeapon);
            else if(StrCmpI(pVar, "Log File") == 0)
            {
                CString strPath;
                Reader.GetString(strPath);
                if(SetLogFile(strPath.c_str()) < 0)
                    CLogger::GetInst().PrintError("Warning! Cannot log to file: %s", strPath.c_str());
            }
            else if(StrCmpI(pVar, "Net Fps") == 0)
                Reader.GetUInt(m_nNetFps);
            else
                CLogger::GetInst().PrintError("Warning! Unknown configuration variable: %s", pVar);
        }
    }
    
    if(!bFoundConf)
        THROW_EXCEPTION("Failed to find server configuration: %s", pszConfName);
}

rfGameType CServerConfig::StrToGameType(const char *pGameType)
{
    if(!StrCmpI(pGameType, "TeamDM"))
        return RF_TEAMDM;

    if(!StrCmpI(pGameType, "CTF"))
        return RF_CTF;

    if(StrCmpI(pGameType, "DM") != 0)
        CLogger::GetInst().PrintError("Warning! Invalid game type: %s", pGameType);

    return RF_DM;
}
