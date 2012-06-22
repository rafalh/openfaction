/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CSpawnpointsManager.h
*  PURPOSE:     Spawnpoints manager
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSPAWNPOINTSMANAGER_H
#define CSPAWNPOINTSMANAGER_H

#include "CSpawnpoint.h"
#include <vector>

class CLevel;

class CSpawnpointsManager
{
    public:
        inline CSpawnpointsManager(CLevel *pLevel):
            m_pLevel(pLevel), m_cRed(0), m_cBlue(0) {}
        
        inline void Reset()
        {
            m_Spawnpoints.resize(0);
            m_cRed = m_cBlue = 0;
        }
        
        inline unsigned GetRedCount() const
        {
            return m_cRed;
        }
        
        inline unsigned GetBlueCount() const
        {
            return m_cBlue;
        }
        
        void Add(CSpawnpoint *pSpawnpoint);
        const CSpawnpoint *Find(bool bTeamSpawn = false, bool bBlueTeam = false) const;
    
    private:
        CLevel *m_pLevel;
        std::vector<CSpawnpoint*> m_Spawnpoints;
        unsigned m_cRed, m_cBlue;
};

#endif // CSPAWNPOINTSMANAGER_H
