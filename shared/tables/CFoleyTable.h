/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CFoleyTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFOLEYTABLE_H
#define CFOLEYTABLE_H

#include <vector>
#include "CString.h"
#include <map>

struct SFoleySound
{
    struct SSound
    {
        CString strFilename;
        float fMinDist;
        float fVolume;
    };
    
    CString strName;
    std::vector<SSound> Sounds;
    CString strMaterial;
};

class CFoleyTable
{
    public:
        int Load(std::istream &Stream);
        const SFoleySound *Get(const CString &strName) const;
    
    private:
        std::map<CString, SFoleySound> m_Sounds;
};

#endif // CFOLEYTABLE_H
