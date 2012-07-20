/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CKillableObject.h
*  PURPOSE:     Base class for all objects with life
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CKILLABLEOBJECT_H
#define CKILLABLEOBJECT_H

#include "CObject.h"

enum EDmgFlags
{
    DMG_EXPLOSION = 1,
    DMG_FALL = 2,
};

struct SDamageInfo
{
    SDamageInfo(): pResponsibleObj(NULL), Flags(0) {}
    
    CObject *pResponsibleObj;
    btVector3 vPos;
    unsigned Flags;
    
    static SDamageInfo Default;
};

class CKillableObject: public CObject
{
    public:
        CKillableObject(EElementType Type, CLevel *pLevel, unsigned nUid = OFE_INVALID_UID);
        virtual void Damage(float fDmg, SDamageInfo &DmgInfo = SDamageInfo::Default);
        
        bool IsAlive() const
        {
            return m_fLife != 0.0f ? true : false;
        }
        
        virtual void Kill(SDamageInfo &DmgInfo = SDamageInfo::Default)
        {
            m_fLife = 0.0f;
        }
        
        virtual float GetLife() const
        {
            return m_fLife;
        }
        
        inline void SetLife(float fLife)
        {
            if(IsAlive() && fLife == 0.0f)
                Kill();
            else
                m_fLife = fLife;
        }
        
    protected:
        float m_fLife;
};

#endif // CKILLABLEOBJECT_H
