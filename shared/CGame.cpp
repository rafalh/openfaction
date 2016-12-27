/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CGame.cpp
*  PURPOSE:     Main class containing references to all useful objects
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CGame.h"
#include "CLevel.h"
#include "CMeshMgr.h"
#include "CAnimMgr.h"
#include "CVirtualFileSystem.h"
#include "util/CException.h"
#include "util/utils.h"
#include "CEventsHandler.h"
#include "CConsole.h"
#include "CString.h"

#include "tables/CTablesMgr.h"
#ifdef OF_CLIENT
# include "CTextureMgr.h"
# include "CSoundManager.h"
#endif // OF_CLIENT

using namespace std;

CEventsHandler CEventsHandler::Default;

CGame::CGame(CConsole *pConsole, irr::IrrlichtDevice *pIrrDevice):
    m_pConsole(pConsole), m_pEventsHandler(&CEventsHandler::Default), m_pIrrDevice(pIrrDevice),
    m_pTablesMgr(nullptr)
{
    m_pMeshMgr = new CMeshMgr(this);
    m_pAnimMgr = new CAnimMgr();
    m_pTablesMgr = new CTablesMgr();
#ifdef OF_CLIENT
    m_pTextureMgr = new CTextureMgr(m_pIrrDevice->getVideoDriver());
    m_pSoundMgr = new CSoundManager(this);
#else
    m_pTextureMgr = NULL;
    m_pSoundMgr = NULL;
#endif // OF_CLIENT

    m_pVfs = &CVirtualFileSystem::GetInst();
    m_pLevel = new CLevel(this);
    
    m_pCamera = NULL;
    
#ifdef OF_CLIENT
    if(m_pIrrDevice)
        m_pIrrDevice->grab();
#endif // OF_CLIENT
}

CGame::~CGame()
{
    delete m_pLevel;
    delete m_pMeshMgr;
    delete m_pAnimMgr;
    delete m_pTablesMgr;
    
#ifdef OF_CLIENT
    delete m_pTextureMgr;
    delete m_pSoundMgr;
    
    if(m_pIrrDevice)
        m_pIrrDevice->drop();
#endif // OF_CLIENT
}

void CGame::InitVfs()
{
#ifndef LINUX
    const CString strRootDir = "C:/games/RedFaction/";
#else
    const CString strRootDir = ""; // current directory
#endif
    
    /* Add custom maps first */
    m_pVfs->AddArchivesDirectory(strRootDir + "user_maps/multi/");
    m_pVfs->AddArchivesDirectory(strRootDir + "user_maps/single/");
    
    /* Add game archives */
    const char *pGameArchives[] = {
#ifdef OF_CLIENT
        "audio.vpp",
#endif // OF_CLIENT
        "levels1.vpp", "levels2.vpp", "levels3.vpp", "levelsm.vpp",
        "maps1.vpp", "maps2.vpp", "maps3.vpp", "maps4.vpp", "maps_en.vpp",
        "meshes.vpp", "motions.vpp",
#ifdef OF_CLIENT
        "music.vpp",
#endif // OF_CLIENT
        "tables.vpp",
#ifdef OF_CLIENT
        "ui.vpp",
#endif // OF_CLIENT
    };
    
    /* Add VPP files to virtual file system */
    for(unsigned i = 0; i < COUNTOF(pGameArchives); ++i)
    {
        m_pVfs->AddArchive(strRootDir + pGameArchives[i]);
        m_pConsole->DbgPrint("Loaded %s\n", pGameArchives[i]);
    }
}

void CGame::LoadTables()
{
    m_pTablesMgr->LoadTables();
}

void CGame::LoadMod(const char *pszModName)
{
    CString strPath;
    strPath.Format("mods/%s/", pszModName);
    m_pVfs->AddArchivesDirectory(strPath);
}

void CGame::LoadLevel(const char *pszFilename)
{
    CVfsFileStream Stream;
    Stream.Open(pszFilename);
    m_pLevel->Load(Stream);
}
