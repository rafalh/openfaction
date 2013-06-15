/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CColWorld.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CColWorld.h"
#include "CProjectile.h"
#include "CEntity.h"
#include "CStaticGeometry.h"
#include "CConsole.h"
#include "CLevel.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#ifdef OF_CLIENT
# include <irrlicht.h>
# include "CGame.h"
# include "camera/CCamera.h"
#endif // OF_CLIENT

using namespace std;
#ifdef OF_CLIENT
using namespace irr;
#endif // OF_CLIENT

#ifdef OF_CLIENT
class COfDebugDrawer: public btIDebugDraw
{
    private:
        int m_DebugMode;
        CGame *m_pGame;
        video::SMaterial m_Material;
    
    public:
        inline COfDebugDrawer(CGame *pGame):
            m_pGame(pGame)
        {
            m_DebugMode =
                //DBG_DrawAabb |
                //DBG_DrawWireframe |
                //DBG_DrawFeaturesText |
                //DBG_DrawContactPoints |
                //DBG_NoDeactivation | 
                //DBG_NoHelpText |
                //DBG_DrawText |
                //DBG_ProfileTimings |
                //DBG_EnableSatComparison |
                //DBG_DisableBulletLCP | 
                //DBG_EnableCCD |
                //DBG_DrawConstraints |
                //DBG_DrawConstraintLimits |
                //DBG_FastWireframe |
            0;
            
            m_Material.setFlag(video::EMF_LIGHTING, false);
        }
        
        bool IsPointVisible(const core::vector3df &vPos)
        {
            const core::vector3df &vCamera = m_pGame->GetCamera()->GetSceneNode()->getPosition();
            if(vCamera.getDistanceFromSQ(vPos) < 1000.0f)
                return true;
            return false;
        }
        
        void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
        {
            core::vector3df vFrom(from.x(), from.y(), from.z());
            core::vector3df vTo(to.x(), to.y(), to.z());
            if(IsPointVisible(vFrom) && IsPointVisible(vTo))
            {
                video::SColor Clr(255, color.x()*255, color.y()*255, color.z()*255);
                m_pGame->GetVideoDriver()->setMaterial(m_Material);
                m_pGame->GetVideoDriver()->setTransform(video::ETS_WORLD, core::IdentityMatrix);
                m_pGame->GetVideoDriver()->draw3DLine(vFrom, vTo, Clr);
            }
        }
        
        void drawBox(const btVector3& bbMin, const btVector3& bbMax, const btVector3& color)
        {
            core::vector3df vMin(bbMin.x(), bbMin.y(), bbMin.z());
            core::vector3df vMax(bbMax.x(), bbMax.y(), bbMax.z());
            if(IsPointVisible(vMin) && IsPointVisible(vMax))
            {
                video::SColor Clr(255, color.x()*255, color.y()*255, color.z()*255);
                m_pGame->GetVideoDriver()->setMaterial(m_Material);
                m_pGame->GetVideoDriver()->setTransform(video::ETS_WORLD, core::IdentityMatrix);
                m_pGame->GetVideoDriver()->draw3DBox(core::aabbox3df(vMin, vMax), Clr);
            }
        }
        
        void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
        {
            // stub
        }
        
        void reportErrorWarning(const char *warningString)
        {
            m_pGame->GetConsole()->Print("reportErrorWarning: %s\n", warningString);
        }
        
        void draw3dText(const btVector3 &location, const char *textString)
        {
            m_pGame->GetConsole()->Print("draw3dText: %s\n", textString);
        }
        
        void setDebugMode(int debugMode)
        {
            m_pGame->GetConsole()->Print("setDebugMode: %x\n", debugMode);
            m_DebugMode = debugMode;
        }
        
        int getDebugMode() const
        {
            return m_DebugMode;
        }
};
#endif // OF_CLIENT

CColWorld::CColWorld(CLevel *pLevel):
    m_PointShape(0.0f)
{
    btVector3 vWorldMin(-1000.0f, -1000.0f, -1000.0f);
    btVector3 vWorldMax(1000.0f, 1000.0f, 1000.0f);
    
    m_pColConf = new btDefaultCollisionConfiguration;
    m_pColDispatcher = new btCollisionDispatcher(m_pColConf);
    m_pBroadPhase = new btAxisSweep3(vWorldMin, vWorldMax);
    //m_pBroadPhase = new btDbvtBroadphase();
    m_pConstraintSolver = new btSequentialImpulseConstraintSolver();
    m_pColWorld = new btDiscreteDynamicsWorld(m_pColDispatcher, m_pBroadPhase, m_pConstraintSolver, m_pColConf);
    
    ((btAxisSweep3*)m_pBroadPhase)->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    
#if defined(DEBUG) && defined(OF_CLIENT)
    static COfDebugDrawer OfDbgDrawer(pLevel->GetGame());
    m_pColWorld->setDebugDrawer(&OfDbgDrawer);
#endif
#if 0
    m_pColWorld->getSolverInfo().m_splitImpulse = true;
	m_pColWorld->getSolverInfo().m_numIterations = 20;
    m_pColWorld->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;
#endif
    
    assert(m_pColWorld->getDispatchInfo().m_useContinuous);
    //gContactAddedCallback = OfContactAddedCallback;
    //gContactProcessedCallback = OfContactProcessedCallback;
}

CColWorld::~CColWorld()
{
    assert(m_pColWorld->getCollisionObjectArray().size() == 0);
    
    delete m_pColWorld;
    m_pColWorld = NULL;
    
    delete m_pBroadPhase;
    m_pBroadPhase = NULL;
    
    delete m_pConstraintSolver;
    m_pConstraintSolver = NULL;
    
    delete m_pColDispatcher;
    m_pColDispatcher = NULL;
    
    delete m_pColConf;
    m_pColConf = NULL;
}

void CColWorld::Update(unsigned nDeltaTime)
{
    m_pColWorld->stepSimulation(nDeltaTime / 1000.0f, 8);
    
    /*int numManifolds = m_pColWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
        btPersistentManifold* contactManifold =  m_pColWorld->getDispatcher()->getManifoldByIndexInternal(i);
        btCollisionObject* pColObj1 = static_cast<btCollisionObject*>(contactManifold->getBody0());
        btCollisionObject* pColObj2 = static_cast<btCollisionObject*>(contactManifold->getBody1());
        CObject * const pObject1 = (CObject*)pColObj1->getUserPointer();
        CObject * const pObject2 = (CObject*)pColObj2->getUserPointer();
        
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
			    //if(pObject1->GetType()==OFET_PROJECTILE || pObject2->GetType()==OFET_PROJECTILE)
                //    printf("Manifold %x %x\n", pObject1->GetUid(), pObject2->GetUid());
			    
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
			}
		}
	}*/
}

struct RayResultCallback: public btCollisionWorld::RayResultCallback
{
    RayResultCallback(bool bClosest, CObject *pIgnoredObj = NULL):
        btCollisionWorld::RayResultCallback(),
        m_pIgnoredObj(pIgnoredObj), m_bClosest(bClosest) {}
    
    virtual bool needsCollision(btBroadphaseProxy *pProxy) const
    {
        if(!btCollisionWorld::RayResultCallback::needsCollision(pProxy))
            return false;
        
        btCollisionObject *pColObj = (btCollisionObject*)pProxy->m_clientObject;
        CObject *pObj = (CObject*)pColObj->getUserPointer();
        return (pObj != m_pIgnoredObj);
    }
    
    virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult &RayResult, bool bNormalInWorldSpace)
    {
        CObject * const pObject = (CObject*)RayResult.m_collisionObject->getUserPointer();
        
        /* Does this object stop rays? */
        if(m_bClosest && (pObject->GetType() != OFET_LEVEL || !((CRoom*)pObject)->IsDetail() || ((CRoom*)pObject)->GetLife() < 0.0f))
        {
            /* Remove further results */
            unsigned i = 0;
            while(i < m_Results.size())
            {
                if(m_Results[i].fFraction > RayResult.m_hitFraction)
                    m_Results.erase(m_Results.begin() + i);
                else
                    ++i;
            }
            
            /* Set new maximal factor */
            m_closestHitFraction = RayResult.m_hitFraction;
        }
            
        m_Results.push_back(SRayResult(pObject, RayResult.m_hitFraction));
        
        return m_closestHitFraction;
    }
    
    inline bool hasHit() const
    {
        return !m_Results.empty();
    }
    
    vector<SRayResult> m_Results;
    CObject *m_pIgnoredObj;
    bool m_bClosest;
};

std::vector<SRayResult> CColWorld::TestRay(const btVector3 &vPos1, const btVector3 &vPos2, short iFilter, bool bClosest, CObject *pIgnoredObj)
{
    RayResultCallback Result(bClosest, pIgnoredObj);
    Result.m_collisionFilterGroup = btBroadphaseProxy::AllFilter;
    Result.m_collisionFilterMask = iFilter;
    m_pColWorld->rayTest(vPos1, vPos2, Result);
    
    return Result.m_Results;
}

struct ContactResultCallback: public btCollisionWorld::ContactResultCallback
{
    ContactResultCallback():
        btCollisionWorld::ContactResultCallback() {}
    
    btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0, int partId0, int index0, const btCollisionObjectWrapper* colObj1, int partId1, int index1)
    {
        m_Objects.push_back((CObject*)(colObj1->getCollisionObject()->getUserPointer()));
        return 1.0f;
    }
    
    inline bool hasHit() const
    {
        return !m_Objects.empty();
    }
    
    vector<CObject*> m_Objects;
};

std::vector<CObject*> CColWorld::TestPoint(const btVector3 &vPos, short iFilter)
{
    btCollisionObject ColPoint;
    ColPoint.setCollisionShape(&m_PointShape);
    ColPoint.setWorldTransform(btTransform(btQuaternion::getIdentity(), vPos));
    
    ContactResultCallback Result;
    Result.m_collisionFilterGroup = btBroadphaseProxy::AllFilter;
    Result.m_collisionFilterMask = iFilter;
    m_pColWorld->contactTest(&ColPoint, Result);
    
    return Result.m_Objects;
}

struct ConvexResultCallback: public btCollisionWorld::ConvexResultCallback
{
    map<CObject*, SColInfo> m_Objects;
    const CObject *m_pIgnoredObj;
    
    ConvexResultCallback(const CObject *pIgnoredObj = NULL):
        m_pIgnoredObj(pIgnoredObj) {}
    
    virtual bool needsCollision(btBroadphaseProxy *pProxy) const
    {
        btCollisionObject *pColObj = (btCollisionObject*)pProxy->m_clientObject;
        CObject *pObj = (CObject*)pColObj->getUserPointer();
        
        if(!btCollisionWorld::ConvexResultCallback::needsCollision(pProxy))
            return false;
        
        return (pObj != m_pIgnoredObj);
    }
    
    virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult &convexResult, bool bNormalInWorldSpace)
    {
        CObject *pObj = (CObject*)convexResult.m_hitCollisionObject->getUserPointer();
        SColInfo &Info = m_Objects[pObj];
        Info.fFraction = convexResult.m_hitFraction;
        Info.vPos = convexResult.m_hitPointLocal;
        Info.vNormal = convexResult.m_hitNormalLocal;
        return convexResult.m_hitFraction;
    }
};

std::map<CObject*, SColInfo> CColWorld::SweepTest(const btTransform &From, const btTransform &To, short iFilter, float fRadius, const CObject *pIgnoredObj)
{
    ConvexResultCallback Result(pIgnoredObj);
    Result.m_collisionFilterGroup = btBroadphaseProxy::AllFilter;
    Result.m_collisionFilterMask = iFilter;
    btSphereShape Shape(fRadius);
    m_pColWorld->convexSweepTest(&Shape, From, To, Result);
    return Result.m_Objects;
}

void CColWorld::AddColObject(CObject *pObj, short iColGroup, short iColMask)
{
    btCollisionObject *pColObj = pObj->GetColObject();
    btRigidBody *pRigidBody = btRigidBody::upcast(pColObj);
    
    if(pRigidBody)
        m_pColWorld->addRigidBody(pRigidBody, iColGroup, iColMask);
    else
        m_pColWorld->addCollisionObject(pColObj, iColGroup, iColMask);
    
    m_pColWorld->updateSingleAabb(pColObj);
}

void CColWorld::RemoveColObject(CObject *pObj)
{
    // removeCollisionObject supports rigid bodies
    m_pColWorld->removeCollisionObject(pObj->GetColObject());
}
