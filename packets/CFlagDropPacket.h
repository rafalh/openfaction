/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFlagDropPacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFLAGDROPPACKET_H
#define CFLAGDROPPACKET_H

class CCaptureTheFlag;
class CPlayer;

class CFlagDropPacket
{
    public:
        inline CFlagDropPacket(CCaptureTheFlag *pCtf, CPlayer *pPlayer):
            m_pCtf(pCtf), m_pPlayer(pPlayer) {}
        
        void Send(CPlayer *pPlayer) const;
        
    private:
        CCaptureTheFlag *m_pCtf;
        CPlayer *m_pPlayer;
};

#endif // CFLAGDROPPACKET_H
