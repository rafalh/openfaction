/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CLightmaps.h
*  PURPOSE:     RFL light-maps loader
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CLIGHTMAPS_H
#define CLIGHTMAPS_H

#include <vector>
#include <irrlicht.h>

class CInputBinaryStream;
class CGame;

class CLightmaps
{
    public:
        CLightmaps(CGame *pGame, CInputBinaryStream &Stream);
        ~CLightmaps();
        void Load(CInputBinaryStream &Stream);
        void Unload();
        
        irr::video::ITexture *Get(unsigned i)
        {
            if(i < m_Textures.size())
                return m_Textures[i];
            return NULL;
        }
        
        unsigned GetCount() const
        {
            return m_Textures.size();
        }
    
    private:
        CGame *m_pGame;
        std::vector<irr::video::ITexture*> m_Textures;
};

#endif // CLIGHTMAPS_H
