/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CObject.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef COBJECT_H
#define COBJECT_H

#include "CElement.h"
#include "CColWorld.h"
#include <btBulletDynamicsCommon.h>
#ifdef OF_CLIENT
# include <irrlicht.h>
#endif

static btVector3 vecZero;
class CObject;

enum EColBits
{
    COL_LEVEL      = 1 << 0,
    COL_ENTITY     = 1 << 1,
    COL_ITEM       = 1 << 2,
    COL_SPAWNPOINT = 1 << 3,
    COL_TRIGGER    = 1 << 4,
    COL_CLUTTER    = 1 << 5,
    COL_PROJECTILE = 1 << 6,
    COL_NONE       = 1 << 15,
};

class CObject: public CElement
{
    public:
        CObject(EElementType Type, CLevel *pLevel, unsigned nUid = OFE_INVALID_UID);
        virtual ~CObject();
        void Update(unsigned nDeltaTime);
        void AddToWorld();
        void RemoveFromWorld();
        
        inline btCollisionObject *GetColObject() const
        {
            return m_pColObj;
        }
        
#ifdef OF_CLIENT
        inline irr::scene::ISceneNode *GetSceneNode()
        {
            return m_pSceneNode;
        }
#endif
        
        inline const btVector3 &GetPos() const
        {
            if(!m_pColObj) return vecZero;
            
            return m_pColObj->getWorldTransform().getOrigin();
        }
        
        virtual void SetPos(const btVector3 &vPos)
        {
            if(!m_pColObj) return;
            
            if(vPos != m_pColObj->getWorldTransform().getOrigin())
            {
                m_pColObj->getWorldTransform().setOrigin(vPos);
                m_bTransformChanged = true;
            }
        }
        
        inline const btMatrix3x3 &GetRotMatrix() const
        {
            if(!m_pColObj) return btMatrix3x3::getIdentity();
            
            return m_pColObj->getWorldTransform().getBasis();
        }
        
        inline void SetRotMatrix(const btMatrix3x3 &matRot)
        {
            if(!m_pColObj) return;
            
            if(!(m_pColObj->getWorldTransform().getBasis() == matRot)) // There is no != operator implemented...
            {
                m_pColObj->getWorldTransform().setBasis(matRot);
                m_bTransformChanged = true;
            }
        }
        
        inline btCollisionShape *GetShape()
        {
            if(!m_pColObj) return NULL;
            return m_pColObj->getCollisionShape();
        }
        
        inline void SetShape(btCollisionShape *pShape)
        {
            m_pColObj->setCollisionShape(pShape);
            m_bTransformChanged = true;
        }
    
    protected:
        btCollisionObject *m_pColObj;
#ifdef OF_CLIENT
        irr::scene::ISceneNode *m_pSceneNode;
#endif
        bool m_bTransformChanged;
        bool m_bIsInWorld;
        short m_iColGroup, m_iColMask;
};

#endif // COBJECT_H
