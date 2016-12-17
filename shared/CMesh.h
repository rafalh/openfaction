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
        void Load(CInputBinaryStream &Stream);
        
        btBvhTriangleMeshShape *GetColShape()
        {
            return m_pColShape;
        }
#ifdef OF_CLIENT
        irr::scene::IMesh *GetIrrMesh(int LodIndex = 0) const
        {
            return m_LodIrrMeshes[LodIndex];
        }

        float GetLodMinDist(int LodIndex) const
        {
            return m_LodDistances[LodIndex];
        }

        int GetLodCount() const
        {
            return m_LodIrrMeshes.size();
        }

        irr::scene::ISceneNodeAnimator *CreateLodAnimator() const;
#endif // OF_CLIENT
    
    private:
        CMeshMgr *m_pMeshMgr;
        btTriangleMesh *m_pColMesh;
        btBvhTriangleMeshShape *m_pColShape;
#ifdef OF_CLIENT
        std::vector<irr::scene::IMesh*> m_LodIrrMeshes;
        std::vector<float> m_LodDistances;
#endif // OF_CLIENT
        std::vector<CMaterial*> m_Materials;
        
        void LoadLodModel(CInputBinaryStream &Stream, bool bColMesh, bool bIrrMesh);
};

class CMesh: public CSharedObject
{
    public:
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
            CString strName;
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
        
        CMesh(CMeshMgr *pMeshMgr = NULL);
        ~CMesh();
        void Load(CInputBinaryStream &Stream);
        void Unload();
        void LoadColSphere(CInputBinaryStream &Stream);
        void LoadBones(CInputBinaryStream &Stream);
        void PrepareBones(int iParent);
    
    friend class CMeshMgr;
};

#endif // CMESH_H
