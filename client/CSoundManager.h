/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CSoundManager.h
*  PURPOSE:     Sound manager
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSOUNDMANAGER_H
#define CSOUNDMANAGER_H

class CGame;

namespace irrklang
{
    class ISoundEngine;
    class IFileFactory;
}

class CSoundManager
{
    public:
        CSoundManager(CGame *pGame);
        ~CSoundManager();
        bool PlayFoleySound(const char *pszName);
        bool PlaySound(const char *pszFilename);
    
    private:
        CGame *m_pGame;
        irrklang::ISoundEngine *m_pSoundEngine;
        irrklang::IFileFactory *m_pFileFactory;
        
        // No copy constructor
        CSoundManager(const CSoundManager&);
};

#endif // CSOUNDMANAGER_H
