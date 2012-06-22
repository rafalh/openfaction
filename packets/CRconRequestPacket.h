/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CRconRequestPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CRCONREQUESTPACKET_H
#define CRCONREQUESTPACKET_H

#include "precomp.h"
#include "CInputBinaryStream.h"

class CRconRequestPacket
{
    public:
        inline CRconRequestPacket(CInputBinaryStream &Stream)
        {
            m_strPassword = Stream.ReadString();
        }
        
        inline const char *GetPassword() const
        {
            return m_strPassword.c_str();
        }
    
    private:
        std::string m_strPassword;
};

#endif // CRCONREQUESTPACKET_H
