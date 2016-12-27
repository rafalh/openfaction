/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/gamemodes/CCaptureTheFlag.h
*  PURPOSE:     CTF gamemode implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCAPTURETHEFLAG_H
#define CCAPTURETHEFLAG_H

#include "precomp.h"
#include "CGamemode.h"
#include "formats/rfproto.h"

class CServer;
struct SItemClass;

class CCaptureTheFlag: public CGamemode
{
    public:
        CCaptureTheFlag(CServer *pServer);
        void Update(unsigned nDeltaTime);
        bool OnItemApply(CItem *pItem, CEntity *pEntity);
        void OnPlayerKill(CPlayer *pPlayer, CPlayer *pKiller);
        void OnPlayerJoin(CPlayer *pPlayer);
        void OnPlayerLeft(CPlayer *pPlayer);
        
        inline unsigned GetId() const
        {
            return RF_CTF;
        }
        
        inline unsigned GetRedScore() const
        {
            return m_RedScore;
        }
        
        inline void SetRedScore(unsigned nScore)
        {
            m_RedScore = nScore;
        }
        
        inline unsigned GetBlueScore() const
        {
            return m_BlueScore;
        }
        
        inline void SetBlueScore(unsigned nScore)
        {
            m_BlueScore = nScore;
        }
        
        inline CPlayer *GetRedFlagPlayer() const
        {
            return m_pRedFlagPlayer;
        }
        
        inline void SetRedFlagPlayer(CPlayer *pPlayer)
        {
            m_pRedFlagPlayer = pPlayer;
        }
        
        inline CPlayer *GetBlueFlagPlayer() const
        {
            return m_pBlueFlagPlayer;
        }
        
        inline void SetBlueFlagPlayer(CPlayer *pPlayer)
        {
            m_pBlueFlagPlayer = pPlayer;
        }
        
        inline bool IsRedFlagDropped() const
        {
            return m_RedFlagDropTimestamp ? true : false;
        }
        
        inline bool IsBlueFlagDropped() const
        {
            return m_BlueFlagDropTimestamp ? true : false;
        }
        
        const btVector3 &GetRedFlagOldPos() const
        {
            return m_vRedFlagOldPos;
        }
        
        const btVector3 &GetBlueFlagOldPos() const
        {
            return m_vBlueFlagOldPos;
        }
        
        static const char *GetName()
        {
            return "ctf";
        }

        // 16-byte alignment is needed for Bullet objects
        BT_DECLARE_ALIGNED_ALLOCATOR()
    
    private:
        unsigned m_RedScore, m_BlueScore;
        time_t m_RedFlagDropTimestamp, m_BlueFlagDropTimestamp;
        btVector3 m_vRedFlagOldPos, m_vBlueFlagOldPos;
        CPlayer *m_pRedFlagPlayer, *m_pBlueFlagPlayer;
        const SItemClass *m_pRedFlagCls, *m_pBlueFlagCls;
        
        bool StealFlag(CPlayer *pPlayer);
        bool CaptureFlag(CPlayer *pPlayer);
        bool DropFlag(CPlayer *pPlayer);
        bool ReturnFlag(bool bBlueFlag, CPlayer *pPlayer = NULL);
};

#endif // CCAPTURETHEFLAG_H
