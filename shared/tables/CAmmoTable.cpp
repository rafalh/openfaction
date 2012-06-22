/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CAmmoTable.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CAmmoTable.h"
#include "CTblReader.h"
#include "CLogger.h"
#include "utils.h"

using namespace std;

SAmmoType::SAmmoType(unsigned _nId)
{
    nId = _nId;
}

int CAmmoTable::Load(istream &Stream)
{
    CTblReader Reader(Stream, "Ammo");
    if(!Reader.IsSectionFound())
        return -1;
    
    while(Reader.LoadNextElement() == 0)
    {
        const char *pName = Reader.GetName();
        
        if(!StrCmpI(pName, "$Name"))
        {
            SAmmoType Ammo(m_AmmoTypes.size());
            Reader.GetString(Ammo.strName);
            
            m_AmmoTypes.push_back(Ammo);
        }
    }
    
    return 0;
}

const SAmmoType *CAmmoTable::Get(const char *pAmmoType) const
{
    for(unsigned i = 0; i < m_AmmoTypes.size(); ++i)
        if(!StrCmpI(m_AmmoTypes[i].strName.c_str(), pAmmoType))
            return &m_AmmoTypes[i];
    
    CLogger::GetInst().PrintError("Warning! Unknown ammo type: %s", pAmmoType);
    return NULL;
}
