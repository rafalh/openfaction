/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CEntitiesTable.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "tables/CEntitiesTable.h"
#include "tables/CTblReader.h"
#include "util/CLogger.h"
#include "util/utils.h"
#include "CString.h"

using namespace std;

SEntityClass::SEntityClass(unsigned _nId)
{
    nId = _nId;
    fMass = 0.0f;
    fCollisionRadius = 0.1f;
    fMovementRadius = 0.1f;
    fEnvirosuit = 100.0f;
    fLife = 100.0f;
}

int CEntitiesTable::Load(istream &Stream)
{
    CTblReader Reader(Stream, "Entity Classes");
    if(!Reader.IsSectionFound())
        return -1;
    
    CString strWeapon;
    
    while(Reader.LoadNextElement() == 0)
    {
        const char *pName = Reader.GetName();
        
        if(!StrCmpI(pName, "$Name"))
        {
            SEntityClass Entity(m_Entities.size());
            Reader.GetString(Entity.strName);
            
            m_Entities.push_back(Entity);
            
            strWeapon.clear();
        }
        else if(!m_Entities.empty())
        {
            if(!StrCmpI(pName, "$V3D Filename"))
                Reader.GetString(m_Entities.back().strMeshFilename);
            else if(!StrCmpI(pName, "$Mass"))
                Reader.GetFloat(m_Entities.back().fMass);
            else if(!StrCmpI(pName, "$Collision Radius"))
                Reader.GetFloat(m_Entities.back().fCollisionRadius);
            else if(!StrCmpI(pName, "$Movement Radius"))
                Reader.GetFloat(m_Entities.back().fMovementRadius);
            else if(!StrCmpI(pName, "$Envirosuit"))
                Reader.GetFloat(m_Entities.back().fEnvirosuit);
            else if(!StrCmpI(pName, "$Life"))
                Reader.GetFloat(m_Entities.back().fLife);
            else if(!StrCmpI(pName, "+Weapon Specific"))
                Reader.GetString(strWeapon);
            else if(!StrCmpI(pName, "+State"))
            {
                CString strState;
                Reader.GetString(strState);
                CEntityState State(strState);
                
                CString strFilename;
                Reader.GetString(strFilename);
                m_Entities.back().States[strWeapon][State] = strFilename;
            }
            else if(!StrCmpI(pName, "+Action"))
            {
                CString strAction;
                Reader.GetString(strAction);
                CEntityAction Action(strAction);
                CString strFilename;
                Reader.GetString(strFilename);
                m_Entities.back().Actions[strWeapon][Action] = strFilename;
            }  
        }
    }
    
    return 0;
}

const SEntityClass *CEntitiesTable::Get(const CString &strClassName) const
{
    for(unsigned i = 0; i < m_Entities.size(); ++i)
        if(!m_Entities[i].strName.comparei(strClassName))
            return &m_Entities[i];
    
    CLogger::GetInst().PrintError("Warning! Unknown entity class name: %s", strClassName.c_str());
    return NULL;
}
