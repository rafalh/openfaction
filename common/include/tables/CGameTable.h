/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CGameTable.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CGAMETABLE_H
#define CGAMETABLE_H

#include "util/CString.h"

class CGameTable
{
    public:
        int Load(std::istream &Stream);
        
        inline const char *GetDefaultWeapon() const
        {
            return strDefaultWeapon;
        }
        
        inline void SetDefaultWeapon(const char *pWeapon)
        {
            strDefaultWeapon = pWeapon;
        }
        
    private:
        CString strDefaultWeapon;
};

#endif // CGAMETABLE_H
