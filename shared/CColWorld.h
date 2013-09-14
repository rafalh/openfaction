/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CColWorld.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCOLWORLD_H
#define CCOLWORLD_H

#include <vector>
#include <map>
#include <btBulletDynamicsCommon.h>

class CObject;
class CLevel;

struct SRayResult
{
    CObject *pObject;
    float fFraction;
    
    inline SRayResult(CObject *pObj, float fFract):
        pObject(pObj), fFraction(fFract) {}
};

struct SColInfo
{
    float fFraction;
    btVector3 vPos, vNormal;
};

class CColWorld
{
    public:
        CColWorld(CLevel *pLevel);
        ~CColWorld();
        void Update(unsigned nDeltaTime);
        void AddColObject(CObject *pObj, short iColGroup = 0, short iColMask = 0);
        void RemoveColObject(CObject *pObj);
        std::vector<SRayResult> TestRay(const btVector3 &vPos1, const btVector3 &vPos2, short iFilter = btBroadphaseProxy::AllFilter, bool bClosest = false, CObject *pIgnoredObj = NULL);
        std::vector<CObject*> TestPoint(const btVector3 &vPos, short iFilter = btBroadphaseProxy::AllFilter);
        std::map<CObject*, SColInfo> SweepTest(const btTransform &From, const btTransform &To, short iFilter = btBroadphaseProxy::AllFilter, float fRadius = 0.0f, const CObject *pIgnoredObj = NULL);
        
        btDiscreteDynamicsWorld *GetColWorld()
        {
            return m_pColWorld;
        }
        
    private:
        btBroadphaseInterface *m_pBroadPhase;
        btDefaultCollisionConfiguration *m_pColConf;
        btCollisionDispatcher *m_pColDispatcher;
        btConstraintSolver *m_pConstraintSolver;
        btDiscreteDynamicsWorld *m_pColWorld;
        btSphereShape m_PointShape;
        
        // No copy constructor
        CColWorld(const CColWorld&);
};

#endif // CCOLWORLD_H
