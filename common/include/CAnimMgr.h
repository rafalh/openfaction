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

#include "util/CString.h"
#include <map>
#include "CAnimation.h"

class CAnimMgr
{
    public:
        ~CAnimMgr();
        CString FixFilename(const CString &strFilename);
        CAnimation *Load(const CString &strFilename);
        
    private:
        std::map<CString, CAnimation*> m_Animations;
        
        void Remove(CAnimation *pAnim);
        
    friend class CAnimation;
};

#endif // CANIMMGR_H
