/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CAnimMgr.h
*  PURPOSE:     Animations manager
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CANIMMGR_H
#define CANIMMGR_H

#include "CString.h"
#include <map>
#include "CAnimation.h"

class CGame;

class CAnimMgr
{
    public:
        CAnimMgr(CGame *pGame):
            m_pGame(pGame) {}
        
        ~CAnimMgr();
        CString FixFilename(const CString &strFilename);
        CAnimation *Load(const CString &strFilename);
        
        CGame *GetGame() const
        {
            return m_pGame;
        }
        
    private:
        std::map<CString, CAnimation*> m_Animations;
        CGame *m_pGame;
        
        void Remove(CAnimation *pAnim);
        
    friend class CAnimation;
};

#endif // CANIMMGR_H
