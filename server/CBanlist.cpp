/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CBanlist.cpp
*  PURPOSE:     Server banlist implementation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "precomp.h"
#include "CBanlist.h"
#include "util/CLogger.h"
#include "CServer.h"
#include "main.h"

using namespace std;

int CBanlist::Load()
{
    FILE *pFile = fopen("banlist.txt", "r");
    if(!pFile)
        return -1;
    
    m_IpAddresses.clear();
    m_IpRanges.clear();
    
    char Buf[32];
    while(fscanf(pFile, "%31s", Buf) == 1)
        Ban(Buf);
    
    fclose(pFile);
    return 0;
}

int CBanlist::Save()
{
    FILE *pFile = fopen("banlist.txt", "w");
    if(!pFile)
        return -1;
    
    for(set<uint32_t>::const_iterator it = m_IpAddresses.begin(); it != m_IpAddresses.end(); ++it)
        fprintf(pFile, "%s\n", CSocketAddress::IpToStr(*it).c_str());
    
    for(vector<SIpRange>::const_iterator it = m_IpRanges.begin(); it < m_IpRanges.end(); ++it)
        fprintf(pFile, "%s-%s\n", CSocketAddress::IpToStr(it->nIp1).c_str(), CSocketAddress::IpToStr(it->nIp2).c_str());
    
    fclose(pFile);
    return 0;
}

int CBanlist::Ban(const char *pIpRange)
{
    const char *pIp2 = strchr(pIpRange, '-');
    
    if(pIp2)
    {
        CString strIp1(pIpRange, pIp2 - pIpRange);
        uint32_t nIp1 = CSocketAddress::StrToIp(strIp1.c_str());
        uint32_t nIp2 = CSocketAddress::StrToIp(pIp2 + 1);
        
        if(!nIp1 || !nIp2)
            return -1;
        
        return BanRange(nIp1, nIp2);
    }
    else
    {
        uint32_t nIp = CSocketAddress::StrToIp(pIpRange);
        if(!nIp)
            return -1;
        
        return Ban(nIp);
    }
}

int CBanlist::Unban(const char *pIpRange)
{
    const char *pIp2 = strchr(pIpRange, '-');
    
    if(pIp2)
    {
        CString strIp1(pIpRange, pIp2 - pIpRange);
        uint32_t nIp1 = CSocketAddress::StrToIp(strIp1.c_str());
        uint32_t nIp2 = CSocketAddress::StrToIp(pIp2 + 1);
        if(!nIp1 || !nIp2)
            return -1;
        
        return UnbanRange(nIp1, nIp2);
    }
    else
    {
        uint32_t nIp = CSocketAddress::StrToIp(pIpRange);
        if(!nIp)
            return -1;
        
        return Unban(nIp);
    }
}

int CBanlist::Ban(uint32_t nIp)
{
    int iResult = m_IpAddresses.insert(nIp).second ? 0 : -1;
    
    for(CPlayer *pPlayer = m_pServer->GetPlayersMgr()->Enum(NULL); pPlayer; pPlayer = m_pServer->GetPlayersMgr()->Enum(pPlayer))
        if(pPlayer->GetAddr().GetAddress() == nIp)
            m_pServer->KickPlayer(pPlayer);
    
    Save();
    
    return iResult;
}
    

int CBanlist::BanRange(uint32_t nIp1, uint32_t nIp2)
{
    for(vector<SIpRange>::const_iterator it = m_IpRanges.begin(); it < m_IpRanges.end(); ++it)
        if(it->nIp1 == nIp1 && nIp2 == it->nIp2)
            return -1;
    
    SIpRange Range = {nIp1, nIp2};
    m_IpRanges.push_back(Range);
    
    for(CPlayer *pPlayer = m_pServer->GetPlayersMgr()->Enum(NULL); pPlayer; pPlayer = m_pServer->GetPlayersMgr()->Enum(pPlayer))
    {
        uint32_t nIp = pPlayer->GetAddr().GetAddress();
        if(nIp1 <= nIp && nIp <= nIp2)
            m_pServer->KickPlayer(pPlayer);
    }
    
    Save();
    
    return 0;
}

int CBanlist::UnbanRange(uint32_t nIp1, uint32_t nIp2)
{
    for(vector<SIpRange>::iterator it = m_IpRanges.begin(); it < m_IpRanges.end(); ++it)
        if(it->nIp1 == nIp1 && nIp2 == it->nIp2)
        {
            m_IpRanges.erase(it);
            Save();
            return 0;
        }
    
    return -1;
}

bool CBanlist::IsBanned(uint32_t nIp) const
{
    if(m_IpAddresses.find(nIp) != m_IpAddresses.end())
        return true;
    
    for(vector<SIpRange>::const_iterator it = m_IpRanges.begin(); it != m_IpRanges.end(); ++it)
        if(it->nIp1 <= nIp && nIp <= it->nIp2)
            return true;
    
    return false;
}
