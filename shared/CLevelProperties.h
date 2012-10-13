/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CLevelProperties.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CLEVELPROPERTIES_H
#define CLEVELPROPERTIES_H

#include "CString.h"
#include "CInputBinaryStream.h"

class CLevel;

class CLevelProperties
{
    public:
        CLevelProperties(CLevel *pLevel);
        void Load(CInputBinaryStream &Stream);
        void Apply(CLevel *pLevel);
        
        bool IsFogEnabled() const
        {
            return fFogFarPlane > 0.0f;
        }
        
        CLevel *m_pLevel;
        CString strGeomodTexture;
        uint32_t uHardness;
        uint32_t AmbientColor;
        uint32_t FogColor;
        float fFogNearPlane, fFogFarPlane;
};

#endif // CLEVELPROPERTIES_H
