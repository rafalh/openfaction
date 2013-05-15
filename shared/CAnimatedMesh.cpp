#include "CAnimatedMesh.h"

CAnimatedMesh::CAnimatedMesh(CMesh *pMesh):
    m_pMesh(pMesh)
{
    assert(m_pMesh);
    assert(m_pMesh->GetSubMeshCount() > 0);
    m_pColShape = m_pMesh->GetSubMesh(0)->GetColShape();
    
#if OF_CLIENT
    irr::scene::IMesh *pIrrMesh = m_pMesh->GetSubMesh(0)->GetIrrMesh();
    m_pIrrMesh = new irr::scene::SAnimatedMesh(pIrrMesh);
    
    for(unsigned i = 0; i < pIrrMesh->getMeshBufferCount(); ++i)
    {
        //irr::scene::IMeshBuffer *pMeshBuf = pIrrMesh->getMeshBuffer(i);
        //TODO
    }
#endif // OF_CLIENT
}

CAnimatedMesh::~CAnimatedMesh()
{
    m_pMesh->Release();
}
