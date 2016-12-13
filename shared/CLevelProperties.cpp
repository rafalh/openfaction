/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CLevelProperties.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CLevelProperties.h"
#include "CLevel.h"
#include "CGame.h"
#include <cassert>

#define RFL_FIX_COLOR(clr) (((clr) & 0xFF00FF00) | (((clr) & 0x00FF0000) >> 16) | (((clr) & 0x000000FF) << 16))

using namespace std;
#ifdef OF_CLIENT
using namespace irr;
#endif

CLevelProperties::CLevelProperties(CLevel *pLevel):
    m_pLevel(pLevel),
    uHardness(0), AmbientColor(0xFFFFFF),
    FogColor(0), fFogNearPlane(0.0f), fFogFarPlane(0.0f) {}

void CLevelProperties::Load(CInputBinaryStream &Stream)
{
    strGeomodTexture = Stream.ReadString2();
    uHardness = Stream.ReadUInt32();
    assert(uHardness <= 100);
    AmbientColor = Stream.ReadUInt32();
    AmbientColor = RFL_FIX_COLOR(AmbientColor);
    assert((AmbientColor >> 24) == 0xFF);
    uint8_t Unknown = Stream.ReadUInt8();
    //printf("level unknown prop %x\n", Unknown);
    assert(Unknown == 0);
    FogColor = Stream.ReadUInt32();
    FogColor = RFL_FIX_COLOR(FogColor);
    assert((FogColor >> 24) == 0xFF);
    fFogNearPlane = Stream.ReadFloat();
    fFogFarPlane = Stream.ReadFloat();
    assert(fFogNearPlane >= 0.0f && fFogFarPlane >= fFogNearPlane);
    
    Apply(m_pLevel);
}

void CLevelProperties::Apply(CLevel *pLevel)
{
#ifdef OF_CLIENT
    m_pLevel->GetGame()->GetSceneMgr()->setAmbientLight(video::SColor(AmbientColor));
    if(fFogFarPlane > 0.0f)
        m_pLevel->GetGame()->GetVideoDriver()->setFog(FogColor, video::EFT_FOG_LINEAR, fFogNearPlane, fFogFarPlane);
#endif // OF_CLIENT
}
