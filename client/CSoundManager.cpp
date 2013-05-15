/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CSoundManager.cpp
*  PURPOSE:     Sound manager
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CSoundManager.h"
#include "CGame.h"
#include "CFoleyTable.h"
#include "CConsole.h"
#include <irrKlang.h>
#include "irr/CFileFactory.h"

CSoundManager::CSoundManager(CGame *pGame):
    m_pGame(pGame)
{
#if 1
    m_pSoundEngine = irrklang::createIrrKlangDevice();
    
    m_pFileFactory = new CIrrKlangFileFactory;
    m_pSoundEngine->addFileFactory(m_pFileFactory);
#else
    m_pSoundEngine = NULL;
    m_pFileFactory = NULL;
#endif
}

CSoundManager::~CSoundManager()
{
    if(m_pSoundEngine)
        m_pSoundEngine->drop();
    if(m_pFileFactory)
        delete m_pFileFactory;
}

bool CSoundManager::PlayFoleySound(const char *pszName)
{
    if(!pszName || !pszName[0] || !m_pSoundEngine)
        return false;
    
    // Find foley sound set
    const SFoleySound *pSndSet = m_pGame->GetFoleyTbl()->Get(pszName);
    if(!pSndSet || pSndSet->Sounds.empty())
        return false;
    
    // Use random sound from the set
    unsigned i = rand() % pSndSet->Sounds.size();
    const SFoleySound::SSound *pSnd = &pSndSet->Sounds[i];
    
    // Create sound object
    irrklang::ISound *pSound = m_pSoundEngine->play2D(pSnd->strFilename, false, true);
    if(!pSound)
    {
        m_pGame->GetConsole()->DbgPrint("Failed to play sound %s", pSnd->strFilename.c_str());
        return false;
    }
    
    // Set sound parameters and start it
    pSound->setVolume(pSnd->fVolume);
    pSound->setIsPaused(false);
    pSound->drop();
    
    return true;
}

bool CSoundManager::PlaySound(const char *pszFilename)
{
    if(!m_pSoundEngine)
        return false;
    
    // Create sound object
    irrklang::ISound *pSound = m_pSoundEngine->play2D(pszFilename, false, true);
    if(!pSound)
    {
        m_pGame->GetConsole()->DbgPrint("Failed to play sound %s", pszFilename);
        return false;
    }
    
    // TODO: Set sound parameters
    pSound->setIsPaused(false);
    pSound->drop();
    
    return true;
}
