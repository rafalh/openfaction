/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  PURPOSE:     V3D/V3M/V3C loader
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#pragma once

#include <vector>
#include <btBulletDynamicsCommon.h>
#include "CInputBinaryStream.h"
#include "CSharedObject.h"

class CMeshMgr;
class CMaterial;
class CObject;

template<typename T>
struct Vector2
{
    T x, y;
    Vector2(T x, T y) : x(x), y(y) {}
};

template<typename T>
struct Vector3
{
    T x, y, z;
    Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
};

typedef Vector2<float> Vector2f;
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

class V3dLodMeshBatch
{
public:
    const Vector3f &getPosition(int VertexIndex) const
    {
        return Vector3f(m_pos[VertexIndex * 3], m_pos[VertexIndex * 3 + 1], m_pos[VertexIndex * 3 + 2]);
    }

    const Vector3f &getNormal(int VertexIndex) const
    {
        return Vector3f(m_norm[VertexIndex * 3], m_norm[VertexIndex * 3 + 1], m_norm[VertexIndex * 3 + 2]);
    }

    const Vector2f &getTexCoord(int VertexIndex) const
    {
        return Vector2f(m_texCoords[VertexIndex * 2], m_texCoords[VertexIndex * 2 + 1]);
    }

    Vector3i getTriangle(int TriIndex) const
    {
        return Vector3i(m_indices[TriIndex * 4], m_indices[TriIndex * 4 + 1], m_indices[TriIndex * 4 + 2]);
    }

    // TODO: bone links

private:
    float *m_pos, *m_norm, *m_texCoords;
    uint16_t *m_indices;
    char *m_data;
};

class V3dLodMesh
{
private:
    std::vector<V3dLodMeshBatch> m_batches;
    std::vector<CString> m_textures;
};

class CSubMesh2
{
    public:
        CSubMesh2(bool bCharacter);
        ~CSubMesh2();
        void Load(CInputBinaryStream &Stream);

        int GetMaterialsCount() const
        {
            return m_cMaterials;
        }
    
    private:
        btTriangleMesh *m_pColMesh;
        btBvhTriangleMeshShape *m_pColShape;
        std::vector<CMaterial*> m_Materials;
        bool m_bIsCharacter;
        int m_cMaterials;
        
        void LoadLodModel(CInputBinaryStream &Stream, bool bColMesh, bool bIrrMesh);
};

class CMesh2: public CSharedObject
{
    public:
#ifdef OF_CLIENT
        void DbgDraw(const CObject *pObj) const;
#endif // OF_CLIENT
        
        CSubMesh2 *GetSubMesh(unsigned i)
        {
            return m_SubMeshes[i];
        }
        
        unsigned GetSubMeshCount() const
        {
            return m_SubMeshes.size();
        }
    
    public:
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

        /*
        struct SLodMesh
        {
            std::vector<SLodMeshBatch> batches;
        };

        struct SSubMesh
        {
            std::vector<SLodMesh> lods;
            std::vector<float> lodDistances;
        };*/
        
        std::vector<CSubMesh2*> m_SubMeshes;
        std::vector<SColSphere> m_ColSpheres;
        std::vector<SBone> m_Bones;
        btMultiSphereShape *m_pMultiColSphere;
        bool m_bIsCharacter;
        
        CMesh2();
        ~CMesh2();
        void Load(CInputBinaryStream &Stream);
        void Unload();
        void LoadColSphere(CInputBinaryStream &Stream);
        void LoadBones(CInputBinaryStream &Stream);
        void PrepareBones(int iParent);
};

