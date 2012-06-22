/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMeshMgr.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CMESHMGR_H
#define CMESHMGR_H

#include <map>
#include <string>

class CMesh;
class CGame;

class CMeshMgr
{
    public:
        CMeshMgr(CGame *pGame):
            m_pGame(pGame) {}
        
        ~CMeshMgr();
        CMesh *Load(const char *pszFilename);
        void Remove(CMesh *pMesh);
        
        CGame *GetGame() const
        {
            return m_pGame;
        }
    
    private:
        CGame *m_pGame;
        std::map<std::string, CMesh*> m_Meshes;
};

#endif // CMESHMGR_H
