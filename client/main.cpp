/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/main.cpp
*  PURPOSE:     Entry point of game client
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include <ctime>
#include <Irrlicht.h>
#include "CClientApp.h"
#include "CVirtualFileSystem.h"
#include "CConsole.h"
#include "CException.h"
#include "CLogger.h"

#include <stdint.h>

using namespace std;
using namespace irr;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
    srand((unsigned)time(NULL));
    CLogger::GetInst().SetPath("of-client.log");
    CLogger::GetInst().PrintStatus("Open Faction started...");
    
    try
    {
        CClientApp *pApp = new CClientApp;
        pApp->Init();
        pApp->LoadLevel("l1s1.rfl");
        //pApp->LoadLevel("test.rfl");
        pApp->Run();
        delete pApp;
    }
    catch(const exception &Exception)
    {
        CLogger::GetInst().PrintError("Fatal error! %s\n", Exception.what());
        return -1;
    }
    
    return 0;
}
