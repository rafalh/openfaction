/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CEvent.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CEVENT_H
#define CEVENT_H

#include "CElement.h"
#include "CInputBinaryStream.h"
#include <vector>

class CEntity;

class CEvent: public CElement
{
    public:
        CEvent(CLevel *pLevel, CInputBinaryStream &Stream);
        bool Activate(CEntity *pEntity);
        void Update(unsigned nDeltaTime);
    
    private:
        std::string m_strClass;
        unsigned m_nWait;
        unsigned m_nDelay;
        std::vector<unsigned> m_Links;
        CEntity *m_pEntity;
        
        bool m_Bool1, m_Bool2;
        int m_Int1, m_Int2;
        float m_Float1, m_Float2;
        std::string m_String1, m_String2;
        
        void Execute();
};

#endif // CEVENT_H
