/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CBanlist.h
*  PURPOSE:     Server banlist implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef _CBANLIST_H
#define _CBANLIST_H

#include "precomp.h"
#include "main.h"
#include "CConsole.h"
#include "CSocket.h"

typedef struct _SIpRange
{
    uint32_t nIp1, nIp2;
} SIpRange;

class CServer;

class CBanlist
{
    public:
        CBanlist(CServer *pServer):
            m_pServer(pServer) {}
        int Load();
        int Save();
        int Ban(const char *pIpRange);
        int Unban(const char *pIpRange);
        int Ban(uint32_t nIp);
        
        inline int Unban(uint32_t nIp)
        {
            int iResult = (m_IpAddresses.erase(nIp) != 0) ? 0 : -1;
            Save();
            return iResult;
        }
        
        int BanRange(uint32_t nIp1, uint32_t nIp2);
        int UnbanRange(uint32_t nIp1, uint32_t nIp2);
        bool IsBanned(uint32_t nIp) const;
        
        inline unsigned GetCount() const
        {
            return m_IpAddresses.size() + m_IpRanges.size();
        }
        
    private:
        CServer *m_pServer;
        std::set<uint32_t> m_IpAddresses;
        std::vector<SIpRange> m_IpRanges;
};

#endif /* _CBANLIST_H */
