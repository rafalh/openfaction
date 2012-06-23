/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CClutterTable.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CClutterTable.h"
#include "CTblReader.h"
#include "CLogger.h"
#include "utils.h"

using namespace std;

SClutterClass::SClutterClass(unsigned _nId)
{
    nId = _nId;
    fHealth = -1.0f;
}

int CClutterTable::Load(istream &Stream)
{
    CTblReader Reader(Stream, "Clutter");
    if(!Reader.IsSectionFound())
        return -1;
    
    while(Reader.LoadNextElement() == 0)
    {
        const char *pName = Reader.GetName();
        
        if(!StrCmpI(pName, "$Class Name"))
        {
            SClutterClass Clutter(m_Clutter.size());
            Reader.GetString(Clutter.strName);
            
            m_Clutter.push_back(Clutter);
        }
        else if(!m_Clutter.empty())
        {
            if(!StrCmpI(pName, "$V3D Filename"))
                Reader.GetString(m_Clutter.back().strMeshFilename);
            else if(!StrCmpI(pName, "$Life"))
                Reader.GetFloat(m_Clutter.back().fHealth);
        }
    }
    
    return 0;
}

const SClutterClass *CClutterTable::Get(const char *pClassName) const
{
    for(unsigned i = 0; i < m_Clutter.size(); ++i)
        if(!StrCmpI(m_Clutter[i].strName.c_str(), pClassName))
            return &m_Clutter[i];
    
    CLogger::GetInst().PrintError("Warning! Unknown clutter class name: %s", pClassName);
    return NULL;
}
