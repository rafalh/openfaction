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

const SAmmoType *CAmmoTable::Get(const CString &strAmmoType) const
{
    for(unsigned i = 0; i < m_AmmoTypes.size(); ++i)
        if(!m_AmmoTypes[i].strName.comparei(strAmmoType))
            return &m_AmmoTypes[i];
    
    CLogger::GetInst().PrintError("Warning! Unknown ammo type: %s", strAmmoType.c_str());
    return NULL;
}
