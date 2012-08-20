#include "CAnimatedMesh.h"

CAnimatedMesh::CAnimatedMesh(CMesh *pMesh):
    m_pMesh(pMesh)
{
    assert(m_pMesh);
    assert(m_pMesh->GetSubMeshCount() > 0);
    irr::scene::IMesh *pIrrMesh = m_pMesh->GetSubMesh(0)->GetIrrMesh();
    m_pIrrMesh = new irr::scene::SAnimatedMesh(pIrrMesh);
    m_pColShape = m_pMesh->GetSubMesh(0)->GetColShape();
    
    for(unsigned i = 0; i < pIrrMesh->getMeshBufferCount(); ++i)
    {
        irr::scene::IMeshBuffer *pMeshBuf = pIrrMesh->getMeshBuffer(i);
        
    }
}

CAnimatedMesh::~CAnimatedMesh()
{
    m_pMesh->Release();
}
