/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CFallDamagePacket.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CFALLDAMAGEPACKET_H
#define CFALLDAMAGEPACKET_H

#include "util/CInputBinaryStream.h"

class CFallDamagePacket
{
    public:
        inline CFallDamagePacket(CInputBinaryStream &Stream)
        {
            m_fForce = Stream.ReadFloat();
        }
        
        inline float GetForce() const
        {
            return m_fForce;
        }
        
    private:
        float m_fForce;
};

#endif // CFALLDAMAGEPACKET_H
