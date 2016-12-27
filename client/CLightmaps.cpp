/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CLightmaps.cpp
*  PURPOSE:     RFL light-maps loader
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CLightmaps.h"
#include "CGame.h"
#include <cassert>
#include <util/CInputBinaryStream.h>

using namespace std;
using namespace irr;

CLightmaps::CLightmaps(CGame *pGame, CInputBinaryStream &Stream):
    m_pGame(pGame)
{
    Load(Stream);
}

CLightmaps::~CLightmaps()
{
    Unload();
}

void CLightmaps::Load(CInputBinaryStream &Stream)
{
    Unload();
    
    uint32_t cLightmaps = Stream.ReadUInt32();
    for(unsigned i = 0; i < cLightmaps; ++i)
    {
        uint32_t w, h;
        w = Stream.ReadUInt32();
        h = Stream.ReadUInt32();
        assert(w < 1024 && h < 1024);
        
        char *pBuf = new char[w * h * 3];
        Stream.ReadBinary(pBuf, w * h * 3);
        video::IImage *pImg = m_pGame->GetVideoDriver()->createImageFromData(video::ECF_R8G8B8, core::dimension2d<u32>(w, h), pBuf, true, false);
        
        char szName[32];
        sprintf(szName, "_LIGHTMAP%u", i);
        video::ITexture *pTexture = m_pGame->GetVideoDriver()->addTexture(szName, pImg);
        m_Textures.push_back(pTexture);
        
        pImg->drop();
        delete[] pBuf;
    }
}

void CLightmaps::Unload()
{
    for(unsigned i = 0; i < m_Textures.size(); ++i)
        m_pGame->GetVideoDriver()->removeTexture(m_Textures[i]);
    m_Textures.clear();
}
