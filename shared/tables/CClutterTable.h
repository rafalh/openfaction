/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CClutterTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCLUTTERTABLE_H
#define CCLUTTERTABLE_H

#include <vector>
#include <string>

struct SClutterClass
{
    SClutterClass(unsigned nId);
    
    unsigned nId;
    std::string strName;
    std::string strMeshFilename;
    float fHealth;
};

class CClutterTable
{
    public:
        int Load(std::istream &Stream);
        const SClutterClass *Get(const char *pClassName) const;
        
        inline const SClutterClass *Get(unsigned nId) const
        {
            if(nId >= m_Clutter.size())
                return NULL;
            
            return &m_Clutter[nId];
        }
    
    private:
        std::vector<SClutterClass> m_Clutter;
};

#endif // CCLUTTERTABLE_H
