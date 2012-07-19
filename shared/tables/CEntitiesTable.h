/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CEntitiesTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CENTITIESTABLE_H
#define CENTITIESTABLE_H

#include <vector>
#include <string>
#include <map>
#include "CEntityState.h"
#include "CEntityAction.h"

struct SEntityClass
{
    SEntityClass(unsigned nId);
    
    unsigned nId;
    std::string strName;
    std::string strMeshFilename;
    float fMass;
    float fCollisionRadius;
    float fMovementRadius;
    float fEnvirosuit;
    float fLife;
    std::map<std::string, std::map<CEntityState, std::string> > States;
    std::map<std::string, std::map<CEntityAction, std::string> > Actions;
};

class CEntitiesTable
{
    public:
        int Load(std::istream &Stream);
        const SEntityClass *Get(const char *pClassName) const;
        
        inline const SEntityClass *Get(unsigned nId) const
        {
            if(nId >= m_Entities.size())
                return NULL;
            
            return &m_Entities[nId];
        }
    
    private:
        std::vector<SEntityClass> m_Entities;
};

#endif // CENTITIESTABLE_H
