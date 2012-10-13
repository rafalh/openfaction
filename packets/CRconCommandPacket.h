/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CRconCommandPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CRCONCOMMANDPACKET_H
#define CRCONCOMMANDPACKET_H

#include "CInputBinaryStream.h"

class CRconCommandPacket
{
    public:
        inline CRconCommandPacket(CInputBinaryStream &Stream)
        {
            m_strCmd = Stream.ReadString();
        }
        
        inline const char *GetCommand() const
        {
            return m_strCmd.c_str();
        }
    
    private:
        CString m_strCmd;
};

#endif // CRCONCOMMANDPACKET_H
