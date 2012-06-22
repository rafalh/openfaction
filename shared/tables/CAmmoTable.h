/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CAmmoTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CAMMOTABLE_H
#define CAMMOTABLE_H

#include <iostream>
#include <string>
#include <vector>

struct SAmmoType
{
    SAmmoType(unsigned nId);
    
    unsigned nId;
    std::string strName;
};

class CAmmoTable
{
    public:
        int Load(std::istream &Stream);
        const SAmmoType *Get(const char *pAmmoType) const;
        
        inline const SAmmoType *Get(unsigned nId) const
        {
            if(nId >= m_AmmoTypes.size())
                return NULL;
            
            return &m_AmmoTypes[nId];
        }
        
        inline unsigned GetCount() const
        {
            return m_AmmoTypes.size();
        }
    
    private:
        std::vector<SAmmoType> m_AmmoTypes;
};

#endif // CAMMOTABLE_H
