/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CElement.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CELEMENT_H
#define CELEMENT_H

enum EElementType
{
    OFET_LEVEL      = 0,
    OFET_ENTITY     = 1,
    OFET_ITEM       = 2,
    OFET_SPAWNPOINT = 3,
    OFET_TRIGGER    = 4,
    OFET_EVENT      = 5,
    OFET_CLUTTER    = 6,
    OFET_PROJECTILE = 7,
    OFET_MAX        = 8
};

#define OFE_INVALID_UID 0xFFFFFFFF

class CLevel;

class CElement
{
    public:
        CElement(EElementType Type, CLevel *pLevel, unsigned nUid = OFE_INVALID_UID);
        virtual ~CElement();
        
        inline EElementType GetType() const
        {
            return m_Type;
        }
        
        inline unsigned GetUid() const
        {
            return m_nUid;
        }
        
        inline CLevel *GetLevel()
        {
            return m_pLevel;
        }
        
        virtual void Update(unsigned nDeltaTime)
        {
        }
        
    private:
        EElementType m_Type;
    
    protected:
        unsigned m_nUid;
        CLevel *m_pLevel;
};

#endif // CELEMENT_H
