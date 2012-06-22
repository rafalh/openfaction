/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CSpawnpoint.cpp
*  PURPOSE:     Spawnpoint object reprezentation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CSpawnpoint.h"

btCylinderShape CSpawnpoint::m_ColShape(btVector3(0.5f, 1.0f, 0.5f));

CSpawnpoint::CSpawnpoint(CLevel *pLevel, unsigned nUid):
    CObject(OFET_SPAWNPOINT, pLevel, nUid),
    m_bRedTeam(true), m_bBlueTeam(true)
{
    m_pColObj = new btCollisionObject();
    m_pColObj->setUserPointer(this);
    m_pColObj->setCollisionShape(&m_ColShape);
    m_pColObj->setCollisionFlags(m_pColObj->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    
    AddToWorld();
}

CSpawnpoint::CSpawnpoint(CLevel *pLevel, CInputBinaryStream &Stream):
    CObject(OFET_SPAWNPOINT, pLevel)
{
    m_pColObj = new btCollisionObject();
    m_pColObj->setUserPointer(this);
    m_pColObj->setCollisionShape(&m_ColShape);
    m_pColObj->setCollisionFlags(m_pColObj->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    
    m_nUid = Stream.ReadUInt32();
    
    SetPos(Stream.ReadVector());
    
    btMatrix3x3 RotMatrix;
    RotMatrix[2] = Stream.ReadVector();
    RotMatrix[0] = Stream.ReadVector();
    RotMatrix[1] = Stream.ReadVector();
    SetRotMatrix(RotMatrix);
    
    Stream.ReadString2(); // script name
    Stream.ignore(5); // zero, team
    
    m_bRedTeam = Stream.ReadUInt8() ? true : false;
    m_bBlueTeam = Stream.ReadUInt8() ? true : false;
    
    Stream.ignore(1); // bot
    
    AddToWorld();
}
