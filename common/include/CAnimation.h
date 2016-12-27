/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CAnimation.h
*  PURPOSE:     Skeletal animation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CANIMATION_H
#define CANIMATION_H

#include "util/CSharedObject.h"
#include "CInputBinaryStream.h"
#include <vector>

class CAnimMgr;

class CAnimation: public CSharedObject
{
    private:
        struct SPosKey
        {
            unsigned Time;
            btVector3 vPos;
        };
        struct SRotKey
        {
            unsigned Time;
            btQuaternion qRot;
        };
        struct SBone
        {
            std::vector<SPosKey> PosKeys;
            std::vector<SRotKey> RotKeys;
        };
        
        std::vector<SBone> m_Bones;
        CAnimMgr *m_pAnimMgr;
        
        CAnimation(CAnimMgr *pAnimMgr):
            m_pAnimMgr(pAnimMgr) {}
        ~CAnimation();
        void Load(CInputBinaryStream &Stream);
        void Unload();
        
    friend class CAnimMgr;
};

#endif // CANIMATION_H
