/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CAnimMgr.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CANIMMGR_H
#define CANIMMGR_H

#include <string>
#include <map>
#include "CAnimation.h"

class CGame;

class CAnimMgr
{
    public:
        CAnimMgr(CGame *pGame):
            m_pGame(pGame) {}
        
        ~CAnimMgr();
        CAnimation *Load(std::string strFilename);
        void Remove(CAnimation *pAnim);
        
        CGame *GetGame() const
        {
            return m_pGame;
        }
        
    private:
        std::map<std::string, CAnimation*> m_Animations;
        CGame *m_pGame;
};

#endif // CANIMMGR_H
