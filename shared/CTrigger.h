/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CTrigger.h
*  PURPOSE:     Trigger element
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CTRIGGER_H
#define CTRIGGER_H

#include "CObject.h"
#include "util/CInputBinaryStream.h"

class CEntity;

class CTrigger: public CObject
{
    public:
        CTrigger(CLevel *pLevel, CInputBinaryStream &Stream);
        
        inline void SetSphereShape(float fRadius)
        {
            if(GetShape())
                delete GetShape();
            SetShape(new btSphereShape(fRadius));
        }
        
        inline void SetBoxShape(const btVector3 &Size)
        {
            if(GetShape())
                delete GetShape();
            SetShape(new btBoxShape(Size / 2.0f)); // btBoxShape uses half extends
        }
        
        inline bool IsUseKeyRequired() const
        {
            return m_bUseKeyRequired;
        }
        
        inline void SetUseKeyRequired(bool bRequired)
        {
            m_bUseKeyRequired = bRequired;
        }
        
        inline void SetResetTime(unsigned nTime)
        {
            m_nResetTime = nTime;
        }
        
        inline void SetResetsCount(unsigned cResets)
        {
            m_cResets = cResets;
        }
        
        inline bool IsEnabled() const
        {
            return m_bEnabled;
        }
        
        //inline void SetEnabled(bool bEnabled)
        //{
        //    m_bEnabled = bEnabled;
        //}
        
        inline bool IsAuto() const
        {
            return m_bAuto;
        }
        
        bool Activate(CEntity *pEntity);
        void Update(unsigned nDeltaTime);
    
    private:
        bool m_bUseKeyRequired;
        unsigned m_nResetTime, m_cResets;
        bool m_bEnabled, m_bAuto;
        unsigned m_nWait;
        std::vector<unsigned> m_Links;
};

#endif // CTRIGGER_H
