/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CItem.h
*  PURPOSE:     Item (pickup) representation, e. g. life, armor, CTF flags
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CITEM_H
#define CITEM_H

#include "CObject.h"
#include "CItemsTable.h"
#include "CInputBinaryStream.h"

class CEntity;
class CMesh;

class CItem: public CObject
{
    public:
        CItem(CLevel *pLevel, const SItemClass *pClass, unsigned nBit = 0xFFFF, unsigned nUid = OFE_INVALID_UID);
        CItem(CLevel *pLevel, CInputBinaryStream &Stream, unsigned nBit = 0xFFFF);
        ~CItem();
        
        inline unsigned GetHandle() const
        {
            return m_Handle;
        }
        
        inline const SItemClass *GetClass() const
        {
            return m_pClass;
        }
        
        inline unsigned GetBit() const
        {
            return m_nBit;
        }
        
        inline unsigned GetCount() const
        {
            return m_nCount;
        }
        
        void SetCount(unsigned nCount)
        {
            m_nCount = nCount;
        }
        
        inline unsigned GetRespawnTime() const
        {
            return m_nRespawnTime;
        }
        
        void SetRespawnTime(unsigned nTime)
        {
            m_nRespawnTime = nTime;
        }
        
        inline bool IsEnabled() const
        {
            return (m_nDisableTime == 0);
        }
        
        void SetEnabled(bool bEnabled);
        bool OnPickup(CEntity *pEntity);
        void Apply(CEntity *pEntity);
        void Update(unsigned nDeltaTime);
        
    private:
        unsigned m_Handle;
        const SItemClass *m_pClass;
        unsigned m_nBit;
        unsigned m_nCount;
        unsigned m_nRespawnTime;
        unsigned m_nDisableTime;
        float m_fRotMulti;
        
        static btSphereShape m_ColShape;
        
        CMesh *m_pMesh;
        
        bool CanApply(CEntity *pEntity);
};

#endif // CITEM_H
