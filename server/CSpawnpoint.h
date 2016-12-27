/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CSpawnpoint.h
*  PURPOSE:     Spawnpoint object reprezentation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSPAWNPOINT_H
#define CSPAWNPOINT_H

#include "CObject.h"
#include "util/CInputBinaryStream.h"

class CSpawnpoint: public CObject
{
    public:
        CSpawnpoint(CLevel *pLevel, unsigned nUid = OFE_INVALID_UID);
        CSpawnpoint(CLevel *pLevel, CInputBinaryStream &Stream);
        
        inline bool IsRedTeam() const
        {
            return m_bRedTeam;
        }
        
        inline void SetRedTeam(bool bRedTeam)
        {
            m_bRedTeam = bRedTeam;
        }
        
        inline bool IsBlueTeam() const
        {
            return m_bBlueTeam;
        }
        
        inline void SetBlueTeam(bool bBlueTeam)
        {
            m_bBlueTeam = bBlueTeam;
        }
        
    private:
        bool m_bRedTeam, m_bBlueTeam;
        
        static btCylinderShape m_ColShape;
};

#endif // CSPAWNPOINT_H
