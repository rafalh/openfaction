/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/main.cpp
*  PURPOSE:     Server entry point
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CServer.h"
#include "precomp.h"
#include "config.h"
#include "utils.h"
#include "version.h"
#include "main.h"
#include "CLock.h"
#include "CBanlist.h"
#include "CLogger.h"
#include "CServerConsole.h"
#include "CTimer.h"
#include "CServerConfig.h"
#include "CVirtualFileSystem.h"
#include "CCmdHandler.h"

using namespace std;

/* Global variables */
CLock g_Lock;
CTimer g_AppTimer;
CServer *g_pServer;

#if !LINUX
BOOL WINAPI InterruptHandler(DWORD)
#else /* LINUX */
void InterruptHandler(int)
#endif /* !LINUX */
{
    g_pServer->Stop();
    CLogger::GetInst().Cleanup();
#if !LINUX
    return TRUE;
#endif /* !LINUX */
}

int ParseCmdLine(int argc, const char *argv[], CServerConfig *pServConfig)
{
    for(int i = 1; i < argc; ++i)
    {
        if(argv[i][0] != '/' && argv[i][0] != '-')
        {
            CLogger::GetInst().PrintError("Invalid option: %s\n", argv[i]);
            return -1;
        }

        const char * const arg = argv[i] + 1;
        if(StrCmpI(arg, "h") == 0 || StrCmpI(arg, "help") == 0 || StrCmpI(arg, "?") == 0)
        {
            CLogger::GetInst().GetConsole()->Print("Usage: %s [-port n] [-spawnwep n] [-log path] [-netfps n] [-conf name]\n"
                            "-port n      Sets server port\n"
                            "-spawnweap n Sets server spawn weapon type\n"
                            "-log path    Enables logging to file\n"
                            "-netfps n    Sets server net FPS\n"
                            "-conf name   Loads specified configuration from dedicated_servers.txt\n"
                            "-mod name    Loads specified mod from mods directory\n", argv[0]);
            return 0;
        }
        else if(StrCmpI(arg, "port") == 0)
        {
            if(i + 1 < argc)
            {
                unsigned uPort = strtoul(argv[++i], NULL, 0);
                if(!pServConfig->SetPort(uPort))
                    CLogger::GetInst().PrintError("Invalid port: %u. Default value will be used.", uPort);
            }
        }
        else if(StrCmpI(arg, "spawnweap") == 0)
        {
            if(i + 1 < argc)
            {
                pServConfig->SetDefaultWeapon(argv[++i]);
            }
        }
        else if(StrCmpI(arg, "log") == 0)
        {
            if(i + 1 < argc)
            {
                const char *pLogPath = argv[++i];
                int iStatus = pServConfig->SetLogFile(pLogPath);
                if(iStatus < 0)
                    CLogger::GetInst().PrintError("Error %d! Failed to enable logging to file: %s", -iStatus, pLogPath);
            }
        }
        else if(StrCmpI(arg, "netfps") == 0)
        {
            if(i + 1 < argc)
            {
                unsigned uNetFps = strtoul(argv[++i], NULL, 0);
                if(!pServConfig->SetNetFps(uNetFps))
                    CLogger::GetInst().PrintError("Invalid net FPS: %u. Default value will be used.", uNetFps);
            }
        }
        else if(StrCmpI(arg, "password") == 0 || StrCmpI(arg, "pw") == 0)
        {
            if(i + 1 < argc)
            {
                pServConfig->SetPassword(argv[++i]);
            }
        }
        else if(StrCmpI(arg, "conf") == 0 || StrCmpI(arg, "dedicated") == 0)
        {
            if(i + 1 < argc)
            {
                const char *pszConfName = argv[++i];
                try
                {
                    pServConfig->Load(pszConfName);
                }
                catch(std::exception &e)
                {
                    CLogger::GetInst().PrintError("Failed to load configuration: %s", e.what());
                }
            }
        }
        else if(StrCmpI(arg, "mod") == 0)
        {
            if(i + 1 < argc)
            {
                const char *pszMod = argv[++i];
                pServConfig->SetMod(pszMod);
            }
        }
        else
            CLogger::GetInst().PrintError("Invalid command-line option: %s", argv[i]);
    }
    
    return 1;
}

int main(int argc, const char *argv[])
{
    // Init console
    CServerConsole *pConsole = new CServerConsole;
    CLogger::GetInst().SetConsole(pConsole);
    
    // Dispaly header
    pConsole->Print(
        "+-------------------------------------------------+\n"
        "|%*s" PRODUCT_NAME " " PRODUCT_VERSION "%*s|\n"
        "+-------------------------------------------------+\n",
        (50 - sizeof(PRODUCT_NAME " " PRODUCT_VERSION) + 1) / 2, "",
        (50 - sizeof(PRODUCT_NAME " " PRODUCT_VERSION)) / 2, "");
    pConsole->Print(PRODUCT_COPYRIGHT "\n");
    pConsole->Print("Initializing...\n");
    
    // Init randomizer
    srand((unsigned)time(NULL));
    
    // Init server configuration
    CServerConfig *pServConfig = new CServerConfig;
    int iRet = ParseCmdLine(argc, argv, pServConfig);
    if(iRet <= 0)
        return iRet;
    
    // Init the server
    g_pServer = new CServer(pServConfig);
    g_pServer->Init();
    
    // Load banlist
    CBanlist *pBanlist = g_pServer->GetBanlist();
    if(pBanlist->Load() >= 0)
        CLogger::GetInst().PrintStatus("Loaded %u %s.", pBanlist->GetCount(), pBanlist->GetCount() != 1 ? "bans" : "ban");
    
    // Load MOD
    if(pServConfig->GetMod())
        g_pServer->GetGame()->LoadMod(pServConfig->GetMod());
    
    /* Set exit handler */
#if !LINUX
    SetConsoleCtrlHandler(InterruptHandler, TRUE);
#else /* LINUX */
    signal(SIGINT, InterruptHandler);
#endif /* !LINUX */
    
    CCmdHandler CmdHandler(pConsole, g_pServer);
    pConsole->SetCmdHandler(&CmdHandler);
    
    pConsole->Print("Ready!\n");
    
    /* Start the server now */
    g_pServer->Start(false);
    
    pConsole->Run();
    
    g_pServer->Stop();
    
    delete pConsole;
    
    return 0;
}
