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

#include "CString.h"
#include <map>
#include "CMaterial.h"

class CGame;

class CMaterialsMgr
{
    public:
        CMaterialsMgr(CGame *pGame):
            m_pGame(pGame) {}
        
        ~CMaterialsMgr();
        CMaterial *Load(const CString &strFilename);
        
        CGame *GetGame() const
        {
            return m_pGame;
        }
        
    private:
        std::map<CString, CMaterial*> m_Materials;
        CGame *m_pGame;
        
        void Remove(CMaterial *pMaterial);
        
    friend class CMaterial;
};

#endif // CMATERIALSMGR_H
