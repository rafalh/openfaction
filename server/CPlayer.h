/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CPlayer.h
*  PURPOSE:     Server client representation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CPLAYER_H
#define CPLAYER_H

#include "precomp.h"
#include "CTimer.h"
#include "CSocket.h"
#include "COutputBinaryStream.h"
#include "CPlayerConnection.h"

class CServer;
class CEntity;
class CPlayerConnection;
struct SMpCharacter;

class CPlayer
{
    public:
        CPlayer(CServer *pServer, const char *pszName, unsigned nId, const CSocketAddress &Addr);
        ~CPlayer();
        
        CServer *GetServer() const
        {
            return m_pServer;
        }
        
        inline void SetName(const char *pszName)
        {
            m_strName = pszName;
        }
        
        inline const char *GetName() const
        {
            return m_strName.c_str();
        }
        
        inline unsigned GetId() const
        {
            return m_nId;
        }
        
        inline unsigned GetPing() const
        {
            return m_nPing;
        }
        
        inline void SetPing(unsigned nPing)
        {
            m_nPing = nPing;
        }
        
        inline unsigned GetCaptures() const
        {
            return m_nCaps;
        }
        
        inline void SetCaptures(unsigned nCaps)
        {
            m_nCaps = nCaps;
        }
        
        inline int GetScore() const
        {
            return m_iScore;
        }
        
        inline void SetScore(int iScore)
        {
            m_iScore = iScore;
        }
        
        inline const CSocketAddress &GetAddr() const
        {
            return m_pConnection->GetAddr();
        }
        
        inline bool IsInGame() const
        {
            return m_bInGame;
        }
        
        inline void SetInGame(bool bInGame)
        {
            m_bInGame = bInGame;
        }
        
        inline void StartPingTimer()
        {
            m_PingTimer.Reset();
        }

        inline void StopPingTimer()
        {
            m_nPing = m_PingTimer.GetValue();
        }
        
        inline bool IsAdmin() const
        {
            return m_bAdmin;
        }
        
        inline void SetAdmin(bool bAdmin)
        {
            m_bAdmin = bAdmin;
        }
        
        inline bool IsBlueTeam() const
        {
            return m_bBlueTeam;
        }
        
        inline void SetBlueTeam(bool bBlueTeam)
        {
            m_bBlueTeam = bBlueTeam;
        }
        
        inline CEntity *GetEntity()
        {
            return m_pEntity;
        }
        
        inline void SetEntity(CEntity *pEntity)
        {
            m_pEntity = pEntity;
        }
        
        inline const SMpCharacter *GetCharacter()
        {
            return m_pCharacter;
        }
        
        inline void SetCharacter(const SMpCharacter *pCharacter)
        {
            m_pCharacter = pCharacter;
        }
        
        CPlayerConnection *GetConnection() const
        {
            return m_pConnection;
        }
        
        CEntity *Spawn(const SMpCharacter *pCharacter, const btVector3 &vPos, const btMatrix3x3 &RotMatrix = btMatrix3x3::getIdentity());
        
        time_t m_Timestamp;
    
    private:
        CServer *m_pServer;
        
        CEntity *m_pEntity;
        const SMpCharacter *m_pCharacter;
        
        std::string m_strName;
        unsigned m_nId, m_nPing, m_nCaps;
        int m_iScore;
        bool m_bInGame;
        
        CTimer m_PingTimer;
        bool m_bAdmin, m_bBlueTeam, m_bFlag;
        
        CPlayerConnection *m_pConnection;
};

#endif // CPLAYER_H
