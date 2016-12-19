/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CShootPacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CShootPacket.h"
#include "CServer.h"
#include "CEntity.h"
#include "CWeapon.h"

CShootPacket::CShootPacket(CPlayer *pPlayer, bool bAltFire, const btVector3 &vPos, const btVector3 &vDir):
    m_pPlayer(pPlayer), m_bAltFire(bAltFire),
    m_bPosDir(false), m_vPos(vPos), m_vDir(vDir),
    m_pWeaponClass(m_pPlayer->GetEntity()->GetWeapon()->GetClass()) {}

CShootPacket::CShootPacket(CInputBinaryStream &Stream, CPlayer *pPlayer):
	m_pPlayer(pPlayer)
{
    m_pWeaponClass = pPlayer->GetServer()->GetGame()->GetTables()->weapons()->Get(Stream.ReadUInt8());
    unsigned nFlags = Stream.ReadUInt8();
    m_bPosDir = (nFlags & RF_SF_NO_POS_ROT) ? false : true;
    m_bAltFire = (nFlags & RF_SF_ALT_FIRE) ? true : false;
    
    if(m_bPosDir)
    {
        m_vPos = Stream.ReadVector();
        
        m_vDir.setX((float)Stream.ReadInt16());
        m_vDir.setY((float)Stream.ReadInt16());
        m_vDir.setZ((float)Stream.ReadInt16());
    }
}

void CShootPacket::Send(CPlayer *pPlayer) const
{
    COutputBinaryStringStream Stream;
    
    Stream.WriteUInt8(m_pWeaponClass->nId);
    unsigned nFlags = 0;
    if(!m_bPosDir)
        nFlags |= RF_SF_NO_POS_ROT;
    if(m_bAltFire)
        nFlags |= RF_SF_ALT_FIRE;
    Stream.WriteUInt8(nFlags);
    Stream.WriteUInt8(m_pPlayer->GetId());
    if(m_bPosDir)
    {
        Stream.WriteVector(m_vPos);
        Stream.WriteInt16((int)m_vDir.x());
        Stream.WriteInt16((int)m_vDir.y());
        Stream.WriteInt16((int)m_vDir.z());
    }
    
    pPlayer->GetConnection()->AddPacket(RF_SHOOT, Stream.str(), false);
}
