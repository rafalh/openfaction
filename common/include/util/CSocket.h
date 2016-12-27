/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/CSocket.h
*  PURPOSE:     Cross-platform socket
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSOCKET_H
#define CSOCKET_H

#include "errors.h"
#include "precomp.h"
#include "CTimer.h"
#include <string>
#include <cstdio>
#include <cstring>
#include <stdint.h>

void DumpBinData(const void *lpData, unsigned cbData, std::FILE *File);

class CSocket;

inline int MakeSureSocketsAreReady()
{
#if !LINUX
    static bool m_bInitWsa = true;

    if(m_bInitWsa)
    {
        WSADATA wsaData;

        if(WSAStartup(MAKEWORD(2, 0), &wsaData))
            return OFE_CANNOT_START_WSA; /* error */

        m_bInitWsa = false;
    }
#endif
    return 0;
}

class CSocketAddress
{
    public:
        inline CSocketAddress(uint32_t nAddr = 0, unsigned nPort = 0)
        {
            memset(m_Sin.sin_zero, 0, sizeof(m_Sin.sin_zero));
            m_Sin.sin_family = AF_INET;

            SetAddress(nAddr);

            m_Sin.sin_port = htons(nPort);
        }

        inline CSocketAddress(const char *pAddr, unsigned nPort)
        {
            memset(m_Sin.sin_zero, 0, sizeof(m_Sin.sin_zero));
            m_Sin.sin_family = AF_INET;

            struct hostent *pHost = gethostbyname(pAddr);
            SetRawAddress(pHost ? ((struct in_addr*)(pHost->h_addr))->s_addr : 0);

            m_Sin.sin_port = htons(nPort);
        }

        inline static uint32_t StrToIp(const char *pAddr)
        {
            MakeSureSocketsAreReady();

            struct hostent *pHost = gethostbyname(pAddr);
            return pHost ? ntohl(((struct in_addr*)(pHost->h_addr))->s_addr) : 0;
        }

        inline static std::string IpToStr(uint32_t nIp)
        {
            MakeSureSocketsAreReady();

            in_addr Addr;
#if !LINUX
            Addr.S_un.S_addr = htonl(nIp);
#else /* LINUX */
            Addr.s_addr = htonl(nIp);
#endif /* !LINUX */
            return inet_ntoa(Addr);
        }

        inline uint32_t GetAddress() const
        {
            return ntohl(GetRawAddress());
        }

        inline void SetAddress(uint32_t nAddr)
        {
            SetRawAddress(htonl(nAddr));
        }

        inline unsigned GetPort() const
        {
            return ntohs(m_Sin.sin_port);
        }

        inline void SetPort(unsigned nPort)
        {
            m_Sin.sin_port = htons(nPort);
        }

        inline const char *GetAddressString() const
        {
            return inet_ntoa(m_Sin.sin_addr);
        }

        inline bool operator==(const CSocketAddress &Addr) const
        {
            return GetAddress() == Addr.GetAddress() && GetPort() == Addr.GetPort();
        }

    private:
        inline uint32_t GetRawAddress() const
        {
#if !LINUX
            return m_Sin.sin_addr.S_un.S_addr;
#else /* LINUX */
            return m_Sin.sin_addr.s_addr;
#endif /* !LINUX */
        }

        inline void SetRawAddress(unsigned long nAddr)
        {
#if !LINUX
            m_Sin.sin_addr.S_un.S_addr = nAddr;
#else /* LINUX */
            m_Sin.sin_addr.s_addr = nAddr;
#endif /* !LINUX */
        }

        sockaddr_in m_Sin;

        friend class CSocket;
};

/* Note: INVALID_SOCKET == SOCKET_ERROR == -1 */

class CSocket
{
    public:
        inline CSocket(): m_Sock(-1) {}

        inline ~CSocket()
        {
            Close();
        }

        inline static int GetLastError()
        {
#if !LINUX
            return WSAGetLastError();
#else /* LINUX */
            return errno; //socket_error
#endif /* !LINUX */
        }

        inline int Recv(void *pBuffer, unsigned cbBuffer)
        {
            return recv(m_Sock, (char*)pBuffer, cbBuffer, 0);
        }

        inline int Send(const void *pBuffer, unsigned cbBuffer)
        {
            return send(m_Sock, (char*)pBuffer, cbBuffer, 0);
        }

        inline int RecvFrom(void *pBuffer, unsigned cbBuffer, CSocketAddress &Addr)
        {
#if !LINUX
            int cbSin = sizeof(Addr.m_Sin);
#else /* LINUX */
            socklen_t cbSin = sizeof(Addr.m_Sin);
#endif /* LINUX */
            int iRet = recvfrom(m_Sock, (char*)pBuffer, cbBuffer, 0, (sockaddr*)&Addr.m_Sin, &cbSin);
            if(iRet > 0)
            {
#ifdef DUMP_PACKETS_SCREEN
                CConsole::GetInst().Print("[%8.2f] RecvFrom (%s:%u):\n", g_AppTimer.GetValue() / 1000.0f, Addr.GetAddressString(), Addr.GetPort());
                DumpBinData(pBuffer, iRet);
#endif
#ifdef DUMP_PACKETS_FILE
                if(iRet > 2 && *((uint16_t*)pBuffer) != 0x2600)
                {
                    FILE *f = fopen("log.txt", "a");
                    fprintf(f, "[%8.2f] RecvFrom (%s:%u):\n", g_AppTimer.GetValue() / 1000.0f, Addr.GetAddressString(), Addr.GetPort());
                    DumpBinData(pBuffer, iRet, f);
                    fprintf(f, "\n");
                    fclose(f);
                }
#endif
            }
            return iRet;
        }

        inline int SendTo(const void *pBuffer, unsigned cbBuffer, const CSocketAddress &Addr)
        {
#ifdef DUMP_PACKETS_SCREEN
            CConsole::GetInst().Print("[%8.2f] SendTo (%s:%u):\n", g_AppTimer.GetValue() / 1000.0f, Addr.GetAddressString(), Addr.GetPort());
            DumpBinData(pBuffer, cbBuffer);
#endif
#ifdef DUMP_PACKETS_FILE
            if(cbBuffer > 2 && *((uint16_t*)pBuffer) != 0x2600 && *((uint16_t*)pBuffer) != 0x2500)
            {
                FILE *f = fopen("log.txt", "a");
                fprintf(f, "[%8.2f] SendTo (%s:%u):\n", g_AppTimer.GetValue() / 1000.0f, Addr.GetAddressString(), Addr.GetPort());
                DumpBinData(pBuffer, cbBuffer, f);
                fprintf(f, "\n");
                fclose(f);
            }
#endif
            return sendto(m_Sock, (char*)pBuffer, cbBuffer, 0, (sockaddr*)&Addr.m_Sin, sizeof(Addr.m_Sin));
        }

        inline int SendTo(const std::string String, const CSocketAddress &Addr)
        {
            return SendTo(String.data(), String.size(), Addr);
        }

        inline int Create()
        {
            MakeSureSocketsAreReady();

            m_Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#if !LINUX
            if(m_Sock == (SOCKET)-1)
#else /* LINUX */
            if(m_Sock == -1)
#endif
                return OFE_CANNOT_CREATE_SOCKET;

            return 0;
        }

        inline void Close()
        {
#if !LINUX
            if(m_Sock != (SOCKET)-1)
#else /* LINUX */
            if(m_Sock != - 1)
#endif
            {
#if !LINUX
                closesocket(m_Sock);
#else /* LINUX */
                close(m_Sock);
#endif /* !LINUX */
                m_Sock = -1;
            }
        }

        inline int Bind(CSocketAddress &Addr)
        {
            if(bind(m_Sock, (sockaddr*)&Addr.m_Sin, sizeof(Addr.m_Sin)) != 0)
                return OFE_CANNOT_BIND_SOCKET;

            return 0;
        }

    private:
#if !LINUX
        SOCKET m_Sock;
        static bool m_bInitWsa;
#else  /* LINUX */
        int m_Sock;
#endif /* !LINUX */

    friend class CSocketAddress;
};

#endif // CSOCKET_H
