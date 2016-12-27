/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CShootPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSHOOTPACKET_H
#define CSHOOTPACKET_H

#include "CPlayer.h"
#include "util/CInputBinaryStream.h"

struct SWeaponClass;

class CShootPacket
{
    public:
        CShootPacket(CPlayer *pPlayer, bool bAltFire = false, const btVector3 &vPos = btVector3(), const btVector3 &vDir = btVector3());
        
        CShootPacket(CInputBinaryStream &Stream, CPlayer *pPlayer);
        void Send(CPlayer *pPlayer) const;
        
        inline CPlayer *GetPlayer() const
        {
            return m_pPlayer;
        }
        
        inline bool IsAltFire() const
        {
            return m_bAltFire;
        }
        
        inline bool HasPosDir() const
        {
            return m_bPosDir;
        }
        
        inline const btVector3 &GetPos() const
        {
            return m_vPos;
        }
        
        inline const btVector3 &GetDir() const
        {
            return m_vDir;
        }
        
        inline const SWeaponClass *GetWeaponClass() const
        {
            return m_pWeaponClass;
        }
    
    private:
        CPlayer *m_pPlayer;
        bool m_bAltFire;
        bool m_bPosDir;
        btVector3 m_vPos, m_vDir;
        const SWeaponClass *m_pWeaponClass;
};

#endif // CSHOOTPACKET_H
