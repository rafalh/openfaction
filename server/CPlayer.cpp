/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CPlayer.cpp
*  PURPOSE:     Server client representation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CPlayer.h"
#include "CServer.h"
#include "CMpCharactersTable.h"
#include "CLevel.h"
#include "CEntity.h"

using namespace std;

CPlayer::CPlayer(CServer *pServer, const char *lpName, unsigned nId, const CSocketAddress &Addr):
    m_pServer(pServer),
    m_pEntity(NULL),
    m_strName(lpName), m_nId(nId),
    m_nPing(0), m_nCaps(0), m_iScore(0),
    m_bInGame(false),
    m_bAdmin(false), m_bBlueTeam(false)
{
    m_pConnection = new CPlayerConnection(this, Addr);
    m_Timestamp = time(NULL);
}

CPlayer::~CPlayer()
{
    delete m_pEntity;
    delete m_pConnection;
}

CEntity *CPlayer::Spawn(const SMpCharacter *pCharacter, const btVector3 &vPos, const btMatrix3x3 &RotMatrix)
{
    m_pCharacter = pCharacter;
    
    if(m_pEntity)
    {
        delete m_pEntity;
        m_pEntity = NULL;
    }
    
    if(!pCharacter->pEntityAnimClass)
        return NULL;
    
    m_pEntity = new CEntity(m_pServer->GetLevel(), pCharacter->pEntityAnimClass);
    m_pEntity->SetPos(vPos);
    m_pEntity->SetRotMatrix(RotMatrix);
    m_pEntity->SetPlayer(this);
    
    return m_pEntity;
}
