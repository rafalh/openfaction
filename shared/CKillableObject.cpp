/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CKillableObject.cpp
*  PURPOSE:     Base class for all objects with life
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CKillableObject.h"

SDamageInfo SDamageInfo::Default;

CKillableObject::CKillableObject(EElementType Type, CLevel *pLevel, unsigned nUid):
    CObject(Type, pLevel, nUid), m_fLife(100.0f) {}

void CKillableObject::Damage(float fDmg, SDamageInfo &DmgInfo)
{
    if(m_fLife > 0.0f && fDmg > 0.0f)
    {
        if(fDmg < m_fLife)
            m_fLife -= fDmg;
        else
            Kill(DmgInfo);
    }
}
