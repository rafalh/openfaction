/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CClutter.h
*  PURPOSE:     Clutter element
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCLUTTER_H
#define CCLUTTER_H

#include "CKillableObject.h"
#include "CInputBinaryStream.h"

class CMesh;
struct SClutterClass;

class CClutter: public CKillableObject
{
    public:
        CClutter(CLevel *pLevel, CInputBinaryStream &Stream);
        ~CClutter();
        void Kill(SDamageInfo &DmgInfo);
        
        const struct SClutterClass *GetClass() const
        {
            return m_pClass;
        }
        
    private:
        const struct SClutterClass *m_pClass;
        
        static btSphereShape m_ColShape;
        btDefaultMotionState m_MotionState;
        
        CMesh *m_pMesh;
};

#endif // CCLUTTER_H
