/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMaterialsMgr.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CMATERIALSMGR_H
#define CMATERIALSMGR_H

#include <string>
#include <map>
#include "CMaterial.h"

class CGame;

class CMaterialsMgr
{
    public:
        CMaterialsMgr(CGame *pGame):
            m_pGame(pGame) {}
        
        ~CMaterialsMgr();
        CMaterial *Load(std::string strFilename);
        void Remove(CMaterial *pMaterial);
        
        CGame *GetGame() const
        {
            return m_pGame;
        }
        
    private:
        std::map<std::string, CMaterial*> m_Materials;
        CGame *m_pGame;
};

#endif // CMATERIALSMGR_H
