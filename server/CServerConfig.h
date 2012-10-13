/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CServerConfig.h
*  PURPOSE:     Reads server configuration
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSERVERCONFIG_H
#define CSERVERCONFIG_H

#include "precomp.h"
#include "rfproto.h"
#include "CLogger.h"

class CServerConfig
{
    public:
        inline CServerConfig():
            m_strName(DEFAULT_NAME),
            m_nGameType(DEFAULT_GAME_TYPE),
            m_nMaxPlayers(DEFAULT_MAX_PLAYERS),
            m_nTimeLimit(DEFAULT_TIME_LIMIT),
            m_nKillsLimit(DEFAULT_KILLS_LIMIT),
            m_nCapturesLimit(DEFAULT_CAPTURES_LIMIT),
            m_nGeomodLimit(DEFAULT_GEOMOD_LIMIT),
            m_bTeamDamage(DEFAULT_TEAM_DAMAGE),
            m_bFallDamage(DEFAULT_FALL_DAMAGE),
            m_bWeaponsStay(DEFAULT_WEAPON_STAY),
            m_bForceRespawn(DEFAULT_FORCE_RESPAWN),
            m_bBalanceTeams(DEFAULT_BALANCE_TEAMS),
            
            m_nPort(DEFAULT_PORT),
            m_bMaskIp(DEFAULT_MASK_IP),
            m_nNetFps(DEFAULT_NET_FPS) {}
        
        void Load(const char *pConfName);
        
        inline const char *GetName() const
        {
            return m_strName.c_str();
        }
        
        inline void SetName(const char *pName)
        {
            m_strName = pName;
        }
        
        inline rfGameType GetGameType() const
        {
            return m_nGameType;
        }
        
        inline unsigned GetMaxPlayers() const
        {
            return m_nMaxPlayers;
        }
        
        inline const char *GetPassword() const
        {
            return m_strPassword.c_str();
        }
        
        inline void SetPassword(const char *pPassword)
        {
            m_strPassword = pPassword;
        }
        
        inline const char *GetRconPassword() const
        {
            return m_strRconPassword.c_str();
        }
        
        inline void SetRconPassword(const char *pPassword)
        {
            m_strRconPassword = pPassword;
        }
        
        inline unsigned GetTimeLimit() const
        {
            return m_nTimeLimit;
        }
        
        inline unsigned GetKillsLimit() const
        {
            return m_nKillsLimit;
        }
        
        inline unsigned GetCapturesLimit() const
        {
            return m_nCapturesLimit;
        }
        
        inline unsigned GetGeomodLimit() const
        {
            return m_nGeomodLimit;
        }
        
        inline bool GetTeamDamage() const
        {
            return m_bTeamDamage;
        }
        
        inline bool GetFallDamage() const
        {
            return m_bFallDamage;
        }
        
        inline bool GetWeaponsStay() const
        {
            return m_bWeaponsStay;
        }
        
        inline bool GetForceRespawn() const
        {
            return m_bForceRespawn;
        }
        
        inline bool GetBalanceTeams() const
        {
            return m_bBalanceTeams;
        }
        
        inline const char *GetLevel(unsigned i) const
        {
            if(m_Levels.size() == 0)
                return DEFAULT_LEVEL;
            
            if(i < m_Levels.size())
                return m_Levels[i].c_str();
        
            return NULL;
        }
        
        inline unsigned GetPort() const
        {
            return m_nPort;
        }
        
        inline bool SetPort(unsigned nPort)
        {
            if(nPort > 65000)
                return false;
            
            m_nPort = nPort;
            return true;
        }
        
        inline bool GetMaskIp() const
        {
            return m_bMaskIp;
        }
        
        inline const char *GetDefaultWeapon() const
        {
            return m_strDefaultWeapon.c_str();
        }
        
        inline void SetDefaultWeapon(const char *pszWeaponClass)
        {
            m_strDefaultWeapon = pszWeaponClass;
        }
        
        inline const char *GetLogFile() const
        {
            return m_strLogFile.c_str();
        }
        
        inline int SetLogFile(const char *pszPath)
        {
            m_strLogFile = pszPath;
            if(pszPath[0])
                return CLogger::GetInst().SetPath(pszPath);
            else
                return CLogger::GetInst().SetPath(NULL);
        }
        
        inline unsigned GetNetFps() const
        {
            return m_nNetFps;
        }
        
        inline bool SetNetFps(unsigned nNetFps)
        {
            if(nNetFps == 0 || nNetFps > 100)
                return false;
            
            m_nNetFps = nNetFps;
            return true;
        }
        
        const char *GetMod() const
        {
            return m_strMod.c_str();
        }
        
        void SetMod(const char *pszMod)
        {
            m_strMod = pszMod;
        }
        
    private:
        static rfGameType StrToGameType(const char *pGameType);
        
        // Standard options
        CString m_strName;
        rfGameType m_nGameType;
        unsigned m_nMaxPlayers;
        CString m_strPassword;
        CString m_strRconPassword;
        unsigned m_nTimeLimit;
        unsigned m_nKillsLimit;
        unsigned m_nCapturesLimit;
        unsigned m_nGeomodLimit;
        bool m_bTeamDamage;
        bool m_bFallDamage;
        bool m_bWeaponsStay;
        bool m_bForceRespawn;
        bool m_bBalanceTeams;
        std::vector<CString> m_Levels;
        
        // Aditional options
        unsigned m_nPort;
        bool m_bMaskIp;
        CString m_strDefaultWeapon;
        CString m_strLogFile;
        unsigned m_nNetFps;
        CString m_strMod;
};

#endif // CSERVERCONFIG_H
