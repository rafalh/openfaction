/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMesh.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CMESH_H
#define CMESH_H

#include <vector>
#include <btBulletDynamicsCommon.h>
#include "CInputBinaryStream.h"
#include "CSharedObject.h"
#ifdef OF_CLIENT
# include <irrlicht.h>
#endif // OF_CLIENT

class CMeshMgr;
class CMaterial;
class CObject;

class CSubMesh
{
    public:
        CSubMesh(CMeshMgr *pMeshMgr);
        ~CSubMesh();
        int Load(CInputBinaryStream &Stream);
        
        btBvhTriangleMeshShape *GetColShape()
        {
            return m_pColShape;
        }
#ifdef OF_CLIENT
        irr::scene::IMesh *GetIrrMesh()
        {
            return m_pIrrMesh;
        }
#endif
    
    private:
        CMeshMgr *m_pMeshMgr;
        btTriangleMesh *m_pColMesh;
        btBvhTriangleMeshShape *m_pColShape;
#ifdef OF_CLIENT
        irr::scene::IMesh *m_pIrrMesh;
#endif
        std::vector<CMaterial*> m_Materials;
        
        int LoadLodModel(CInputBinaryStream &Stream, bool bColMesh, bool bIrrMesh);
};

class CMesh: public CSharedObject
{
    public:
        CMesh(CMeshMgr *pMeshMgr = NULL);
        ~CMesh();
        void Load(CInputBinaryStream &Stream);
        void Unload();
        btMultiSphereShape *GetMultiColSphere();
#ifdef OF_CLIENT
        void DbgDraw(const CObject *pObj) const;
#endif // OF_CLIENT
        
        CSubMesh *GetSubMesh(unsigned i)
        {
            return m_SubMeshes[i];
        }
        
        unsigned GetSubMeshCount() const
        {
            return m_SubMeshes.size();
        }
    
    private:
        struct SBone
        {
            std::string strName;
            btQuaternion qRot;
            btVector3 vPos;
            int iParent;
            
            btTransform BoneToModelTransform;
            btTransform ModelToBoneTransform;
            //btTransform ModelToBindPoseBoneTransform;
        };
        
        struct SColSphere
        {
            btVector3 vPos;
            float fRadius;
            int iBone;
        };
        
        std::vector<CSubMesh*> m_SubMeshes;
        std::vector<SColSphere> m_ColSpheres;
        std::vector<SBone> m_Bones;
        btMultiSphereShape *m_pMultiColSphere;
        CMeshMgr *m_pMeshMgr;
        
        void LoadColSphere(CInputBinaryStream &Stream);
        void LoadBones(CInputBinaryStream &Stream);
        void PrepareBones(int iParent);
};

#endif // CMESH_H
