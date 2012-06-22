/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CSpawnpointsManager.cpp
*  PURPOSE:     Spawnpoints manager
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CSpawnpointsManager.h"
#include "CServer.h"
#include "CLevel.h"

using namespace std;

void CSpawnpointsManager::Add(CSpawnpoint *pSpawnpoint)
{
    m_Spawnpoints.push_back(pSpawnpoint);
    
    if(pSpawnpoint->IsRedTeam())
        ++m_cRed;
    
    if(pSpawnpoint->IsBlueTeam())
        ++m_cBlue;
}

const CSpawnpoint *CSpawnpointsManager::Find(bool bTeamSpawn, bool bBlueTeam) const
{
    /* Make a copy so we can remove bad spawnpoints from it */
    vector<CSpawnpoint*> Spawnpoints = m_Spawnpoints;
    
    /* Check spawnpoints teams */
    if(bTeamSpawn)
    {
        unsigned i = 0;
        while(i < Spawnpoints.size())
        {
            if((bBlueTeam && Spawnpoints[i]->IsBlueTeam()) || (!bBlueTeam && Spawnpoints[i]->IsRedTeam()))
                Spawnpoints.erase(Spawnpoints.begin() + i);
            else
                ++i;
        }
    }
    
    /* Find spawnpoint not blocked by any entity */
    while(!Spawnpoints.empty())
    {
        unsigned i = rand() % Spawnpoints.size();
        if(m_pLevel->GetColWorld()->TestPoint(Spawnpoints[i]->GetPos(), 1 << OFET_ENTITY).empty())
            return Spawnpoints[i];
        
        Spawnpoints.erase(Spawnpoints.begin() + i);
    }
    
    return NULL;
}
