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
#include <string>
#include <map>

struct SFoleySound
{
    struct SSound
    {
        std::string strFilename;
        float fMinDist;
        float fVolume;
    };
    
    std::string strName;
    std::vector<SSound> Sounds;
    std::string strMaterial;
};

class CFoleyTable
{
    public:
        int Load(std::istream &Stream);
        const SFoleySound *Get(const char *pszName) const;
    
    private:
        std::map<std::string, SFoleySound> m_Sounds;
};

#endif // CFOLEYTABLE_H
