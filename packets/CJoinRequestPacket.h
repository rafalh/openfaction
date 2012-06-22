/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CJoinRequestPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CJOINREQUESTPACKET_H
#define CJOINREQUESTPACKET_H

#include "precomp.h"
#include "CInputBinaryStream.h"

class CJoinRequestPacket
{
    public:
        CJoinRequestPacket(CInputBinaryStream &Stream);
        
        inline unsigned GetVersion() const
        {
            return m_nVersion;
        }
        
        inline const char *GetName() const
        {
            return m_strName.c_str();
        }
        
        inline const char *GetPassword() const
        {
            return m_strPassword.c_str();
        }
        
        inline uint32_t GetTablesVppChecksum() const
        {
            return m_nTablesVppChecksum;
        }
        
        inline uint32_t GetTablesVppSize() const
        {
            return m_cbTablesVpp;
        }
    
    private:
        unsigned m_nVersion, m_cbTablesVpp;
        uint32_t m_nTablesVppChecksum;
        std::string m_strName, m_strPassword;
};

#endif // CJOINREQUESTPACKET_H
