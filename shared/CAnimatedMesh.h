#ifndef CANIMATEDMESH_H
#define CANIMATEDMESH_H

#include "CMesh.h"

class CAnimatedMesh
{
    public:
        CAnimatedMesh(CMesh *pMesh);
        ~CAnimatedMesh();
        
#ifdef OF_CLIENT
        void DbgDraw(const CObject *pObj) const
        {
            m_pMesh->DbgDraw(pObj);
        }
#endif // OF_CLIENT
        
        btMultiSphereShape *GetMultiColSphere()
        {
            return m_pMesh->GetMultiColSphere();
        }
        
        btBvhTriangleMeshShape *GetColShape()
        {
            return m_pColShape;
        }
        
#ifdef OF_CLIENT
        irr::scene::IMesh *GetIrrMesh()
        {
            return m_pIrrMesh;
        }
#endif // OF_CLIENT
    
    private:
        // No copy constructor
        CAnimatedMesh(const CAnimatedMesh&);
        
        CMesh *m_pMesh;
#ifdef OF_CLIENT
        irr::scene::IMesh *m_pIrrMesh;
#endif // OF_CLIENT
        btBvhTriangleMeshShape *m_pColShape;
};

#endif // CANIMATEDMESH_H
