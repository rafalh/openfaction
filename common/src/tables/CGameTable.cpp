/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CGameTable.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "tables/CGameTable.h"
#include "tables/CTblReader.h"
#include "utils.h"

using namespace std;

int CGameTable::Load(istream &Stream)
{
    CTblReader Reader(Stream);
    
    while(Reader.LoadNextElement() == 0)
    {
        const char *pName = Reader.GetName();
        
        if(StrCmpI(pName, "$Default Player Weapon") == 0)
            Reader.GetString(strDefaultWeapon);
    }
    
    return 0;
}
