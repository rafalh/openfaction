/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CServerConsole.cpp
*  PURPOSE:     Server console implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CServer.h"
#include "version.h"
#include "CServerConfig.h"
#include "CBanlist.h"
#include "CPlayer.h"
#include "CMessagePacket.h"
#include "tables/CGameTable.h"
#include "utils.h"
#include "CServerConsole.h"
#include "CLock.h"
#include "CLevel.h"
#include "CVirtualFileSystem.h"

using namespace std;

const char *CServerConsole::m_pCmdPrompt = ""; //"> "

void CServerConsole::Run()
{
    Print(m_pCmdPrompt);
    
    char Buffer[256];
    while(fgets(Buffer, sizeof(Buffer), stdin))
    {
        /* Terminate Buffer with '\0' at the end of command and find arg */
        char *pArg = Buffer;
        while(*pArg && !isspace(*pArg))
            ++pArg;
        while(isspace(*pArg))
            *(pArg++) = '\0';

        /* Remove '\n' from pArg */
        char *Ptr = pArg;
        while(*Ptr && *Ptr != '\n')
            ++Ptr;
        *Ptr = '\0';

        /* Execute command */
        if(Buffer[0] && !ExecuteCommand(Buffer, pArg))
            break;

        Print(m_pCmdPrompt);
    }
}

void CServerConsole::CmdOutput(TUser hUser, const char *pszFormat, ...)
{
    va_list pArgList;
    va_start(pArgList, pszFormat);
    
    if(!hUser)
    {
        VPrint(pszFormat, pArgList);
        Print("\n");
    }
    else
    {
#if SEND_RCON_OUTPUT
        char Buf[RF_MAX_MSG_LEN + 1];
        vsnprintf(Buf, RF_MAX_MSG_LEN, pszFormat, pArgList);
        CMessagePacket(Buf).Send((CPlayer*)hUser);
#endif
    }
    
    va_end(pArgList);
}
