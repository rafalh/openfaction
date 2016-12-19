/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CMesh2.h"
#include "v3d_format.h"
//#include "CConsole.h"
//#include "CLevelProperties.h"
//#include "CMeshMgr.h"
//#include "CMaterialsMgr.h"
//#include "CGame.h"
//#include "CObject.h"
#include "CException.h"
#ifdef OF_CLIENT
# include "irr/CReadFile.h"
# include "CLevel.h"
# include "camera/CCamera.h"
#include "irr/CLodSceneNodeAnimator.h"
#endif // OF_CLIENT

//#define USE_LOD_MODELS
#undef OF_CLIENT

#define ALIGN(num, alignment) (((num) + (alignment) - 1) - ((num) + (alignment) - 1) % (alignment))
#define ASSERT assert

#define V3D_ALIGN_UP(val) ALIGN(val, V3D_ALIGNMENT)
//#define V3D_DBGPRINT m_pMeshMgr->GetGame()->GetConsole()->DbgPrint
#define V3D_DBGPRINT printf

bool isAsciiText(const CString &str)
{
    for (char ch : str)
        if (ch <= 0x8 || ch >= 0x7F)
            return false;
    return true;
}

bool isPosVector(const btVector3 &vec)
{
    return (
        vec.x() > -1000.0f && vec.x() < 1000.0f &&
        vec.y() > -1000.0f && vec.y() < 1000.0f &&
        vec.z() > -1000.0f && vec.z() < 1000.0f);
}

using namespace std;
#ifdef OF_CLIENT
using namespace irr;
#endif // OF_CLIENT

CMesh2::CMesh2():
    m_pMultiColSphere(NULL) {}

CMesh2::~CMesh2()
{
    Unload();
    //if(m_pMeshMgr)
    //    m_pMeshMgr->Remove(this);
}

void CMesh2::Load(CInputBinaryStream &Stream)
{
    Unload();
    
    v3d_header_t Hdr = {0, 0, 0};
    Stream.ReadBinary(&Hdr, sizeof(Hdr));
    ASSERT(Hdr.submeshes_count > 0 && Hdr.submeshes_count < 32);
    ASSERT(Hdr.colspheres_count < 32);
    ASSERT(Hdr.total_triangles_count == 0 && Hdr.total_vertices_count == 0 &&
        Hdr.unknown0 == 0 && Hdr.unknown2 == 0 && Hdr.unknown3 == 0);
    ASSERT(Hdr.total_materials_count > 0);
    
    if (Hdr.signature != V3M_SIGNATURE && Hdr.signature != V3C_SIGNATURE)
        THROW_EXCEPTION("Wrong V3D signature: 0x%x\n", Hdr.signature);
    m_bIsCharacter = (Hdr.signature == V3C_SIGNATURE);
    
    ASSERT(Hdr.version == V3D_VERSION);
    if(Hdr.version != V3D_VERSION)
        V3D_DBGPRINT("Unknown V3D version: 0x%x\n", Hdr.version);
    
    int cTotalMaterials = 0;
    v3d_section_t Sect;
    while(Stream.good())
    {
        // Read section header
        //streampos nPos = Stream.tellg();
        Stream.ReadBinary(&Sect, sizeof(Sect));
        
        // Is it ending section?
        if(Sect.type == V3D_END)
            break;
        
        //m_pMeshMgr->GetGame()->GetConsole()->DbgPrint("Section 0x%x at 0x%x\n", Sect.type, nPos);
        
        switch(Sect.type)
        {
            case V3D_SUBMESH:
            {
                CSubMesh2 *pSubMesh = new CSubMesh2(m_bIsCharacter);
                pSubMesh->Load(Stream);
                // Some models have more than one submesh, see APC.v3m
                // FIXME: properly support submeshes
                m_SubMeshes.push_back(pSubMesh);
                cTotalMaterials += pSubMesh->GetMaterialsCount();
                break;
            }
            case V3D_COLSPHERE:
                LoadColSphere(Stream);
                break;
            case V3D_BONE:
                ASSERT(Hdr.signature == V3C_SIGNATURE); // only V3C has bones
                LoadBones(Stream);
                break;
            default:
                //m_pMeshMgr->GetGame()->GetConsole()->DbgPrint("Unknown section 0x%x\n", Sect.type);
                ASSERT(false);
                Stream.ignore(Sect.size);
        }
    }
    
    ASSERT(Stream);
    //if (m_SubMeshes.size() > 1)
    //    V3D_DBGPRINT("Multiple submeshes %d\n", m_SubMeshes.size());
    ASSERT(m_SubMeshes.size() == Hdr.submeshes_count);
    ASSERT(m_ColSpheres.size() == Hdr.colspheres_count);
    ASSERT(cTotalMaterials == Hdr.total_materials_count);
}

void CMesh2::Unload()
{
    for(unsigned i = 0; i < m_SubMeshes.size(); ++i)
        delete m_SubMeshes[i];
    m_SubMeshes.clear();
    
    delete m_pMultiColSphere;
    m_pMultiColSphere = NULL;
}

void CMesh2::LoadColSphere(CInputBinaryStream &Stream)
{
    // Read colsphere from stream
    SColSphere ColSphere;
    char name[24];
    Stream.ReadBinary(name, sizeof(name));
    ASSERT(isAsciiText(name));
    
    ColSphere.iBone = Stream.ReadInt32();
    assert(ColSphere.iBone >= -1 && ColSphere.iBone < 100);
    ColSphere.vPos = Stream.ReadVector();
    ColSphere.fRadius = Stream.ReadFloat();
    assert(ColSphere.fRadius >= 0.0f && ColSphere.fRadius < 100.0f);
    
    // Add colsphere to internal arrays
    m_ColSpheres.push_back(ColSphere);
}

void CMesh2::LoadBones(CInputBinaryStream &Stream)
{
    int Root = -1;
    
    unsigned cBones = Stream.ReadInt32();
    for(unsigned i = 0; i < cBones; ++i)
    {
        SBone Bone;
        char szName[25];
        
        Stream.read(szName, 24);
        szName[24] = 0;
        Bone.strName = szName;
        ASSERT(isAsciiText(szName));
        Bone.qRot = Stream.ReadQuaternion();
        Bone.vPos = Stream.ReadVector();
        Bone.iParent = Stream.ReadInt32();
        
        //V3D_DBGPRINT("%s (parent %d)\n\tpos: %.1f %.1f %.1f\n", Bone.strName.c_str(), Bone.iParent, Bone.vPos[0], Bone.vPos[1], Bone.vPos[2]);
        //V3D_DBGPRINT("\trot: %.1f %.1f %.1f %.1f\n", Bone.qRot[0], Bone.qRot[1], Bone.qRot[2], Bone.qRot[3]);
        
        if (Bone.iParent == -1)
        {
            ASSERT(Root == -1); // only one root
            Root = i;
        }
        
        m_Bones.push_back(Bone);
    }
    
    ASSERT(Root >= 0);
    
    for(unsigned i = 0; i < m_Bones.size(); ++i)
        ASSERT(m_Bones[i].iParent == -1 || (m_Bones[i].iParent >= 0 && m_Bones[i].iParent < (int)m_Bones.size()));
    
    PrepareBones(Root);
}

void CMesh2::PrepareBones(int iParent)
{
    for(unsigned i = 0; i < m_Bones.size(); ++i)
    {
        SBone &Bone = m_Bones[i];
            
        Bone.ModelToBoneTransform = btTransform(Bone.qRot.inverse(), Bone.vPos);
        Bone.BoneToModelTransform = Bone.ModelToBoneTransform.inverse();
    }
}

CSubMesh2::CSubMesh2(bool bCharacter):
    m_pColMesh(NULL), m_pColShape(NULL), m_bIsCharacter(bCharacter),
    m_cMaterials(0)
{}

CSubMesh2::~CSubMesh2()
{
    // Cleanup Bullet collision mesh and shape
    if(m_pColMesh)
        delete m_pColMesh;
    m_pColMesh = NULL;
    
    if(m_pColShape)
        delete m_pColShape;
    m_pColShape = NULL;
    
    // Cleanup materials
    //for(unsigned i = 0; i < m_Materials.size(); ++i)
    //    m_Materials[i]->Release();
    m_Materials.clear();
}

void CSubMesh2::Load(CInputBinaryStream &Stream)
{
    char name[24], name2[24];
    Stream.ReadBinary(name, sizeof(name));
    Stream.ReadBinary(name2, sizeof(name2));
    ASSERT(isAsciiText(name));
    ASSERT(name2 == CString("None") || name2 == CString(name));
    V3D_DBGPRINT("Submesh %s\n", name);
    
    uint32_t uVer = Stream.ReadUInt32();
    ASSERT(uVer == 7);
    
    uint32_t cLodModels = Stream.ReadUInt32();
    ASSERT(cLodModels >= 1 && cLodModels <= 3);
    float fPrevDist;
    for(unsigned i = 0; i < cLodModels; ++i)
    {
        float fDist = Stream.ReadFloat();// LOD distances
        if(i == 0)
            ASSERT(fDist == 0.0f);
        else
            ASSERT(fDist == fPrevDist || fDist > fPrevDist + 1.0f); // some models have 3 x 0.0f (see grab.v3c)
        fPrevDist = fDist;
    }
    
    btVector3 vCenter = Stream.ReadVector();
    ASSERT(isPosVector(vCenter));
    float fRadius = Stream.ReadFloat();
    ASSERT(fRadius > 0.0f && fRadius < 100.0f);
    
    btVector3 vAabbMin = Stream.ReadVector();
    btVector3 vAabbMax = Stream.ReadVector();
    ASSERT(isPosVector(vAabbMin) && isPosVector(vAabbMax));
    ASSERT(vAabbMin.x() <= vAabbMax.x() && vAabbMin.y() <= vAabbMax.y() && vAabbMin.z() <= vAabbMax.z());
    
    for(unsigned i = 0; i < cLodModels; ++i)
    {
        V3D_DBGPRINT("LOD model %u at 0x%x\n", i, Stream.tellg());
        LoadLodModel(Stream, true, true);
    }
    
    uint32_t cMaterials = Stream.ReadUInt32();
    ASSERT(cMaterials < 32);
    std::vector<v3d_material_t> Materials(cMaterials);
    Stream.ReadBinary(Materials.data(), cMaterials * sizeof(v3d_material_t));
    for (unsigned i = 0; i < cMaterials; ++i)
    {
        V3D_DBGPRINT("  >>> Material 0x%X diffmap %s refmap %s flags 0x%02X unk %.2f %.2f %.2f %.2f\n",
            i, Materials[i].diffuse_map_name, Materials[i].ref_map_name, Materials[i].flags,
            Materials[i].unknown[0], Materials[i].unknown[1], Materials[i].unknown[2], Materials[i].unknown[3]);
        ASSERT(strlen(Materials[i].diffuse_map_name) > 4 && isAsciiText(Materials[i].diffuse_map_name));
        ASSERT(Materials[i].ref_map_name[0] == 0 || (strlen(Materials[i].ref_map_name) > 4 && isAsciiText(Materials[i].ref_map_name)));
        ASSERT(Materials[i].flags <= 0x1F);
        //ASSERT(Materials[i].flags == 0x1 || Materials[i].flags == 0x9 || Materials[i].flags == 0x11 || Materials[i].flags == 0x19);
        ASSERT(Materials[i].unk_cof >= 0.0f && Materials[i].unk_cof <= 1.0f);
        ASSERT(Materials[i].unknown[0] == 0.0f && Materials[i].unknown[1] == 0.0f);
        ASSERT(Materials[i].ref_cof >= 0.0f && Materials[i].ref_cof <= 1.0f);
        ASSERT(Materials[i].ref_cof == 0.0f || Materials[i].ref_map_name[0]);
    }
    m_cMaterials = cMaterials;
    
    uint32_t cUnknown = Stream.ReadUInt32();
    ASSERT(cUnknown == 1);
    
    for (unsigned i = 0; i < cUnknown; ++i)
    {
        char unkName[24];
        Stream.ReadBinary(unkName, sizeof(unkName));
        float unkFl = Stream.ReadFloat();
        ASSERT(CString(unkName) == CString(name) && unkFl == 0.0f);
    }
    //Stream.ignore(cUnknown * 28); // unknown4
    
    ASSERT(Stream);
}

void CSubMesh2::LoadLodModel(CInputBinaryStream &Stream, bool bColMesh, bool bIrrMesh)
{
    uint32_t uFlags = Stream.ReadUInt32();
    ASSERT(uFlags <= 0x33);
    ASSERT((uFlags & 0x2F) == (m_bIsCharacter ? 0x03 : 0x20));
    // Note: flag 0x10 is used by driller01.v3m only
    
    uint32_t Unknown0 = Stream.ReadUInt32();
    ASSERT(Unknown0 < 1000);
    
    uint16_t cBatches = Stream.ReadUInt16();
    ASSERT(cBatches > 0 && cBatches < 256);
    
    uint32_t cbData = Stream.ReadUInt32();
    ASSERT(cbData > cBatches * 56u && cbData < 0x1000000u); // 1MB
    
    //V3D_DBGPRINT("Flags 0x%x cVertices %u cBatches %u\n", uFlags, cVertices, cBatches);
    
    unsigned uDataOffset = (unsigned)Stream.tellg();
    //V3D_DBGPRINT("Data starts at 0x%x\n", uDataOffset);
    
    char *pData = new char[cbData];
    Stream.ReadBinary(pData, cbData);
    
    //V3D_DBGPRINT("Data ends at 0x%x\n", Stream.tellg());
    
    int32_t Unknown = Stream.ReadInt32(); // unknown
    ASSERT(Unknown == -1 || Unknown == 0);
    
    std::vector<v3d_batch_info_t> Batches(cBatches);
    unsigned cVerifyTotalVertices = 0, cVerifyTotalTriangles = 0;
    Stream.ReadBinary(Batches.data(), cBatches * sizeof(v3d_batch_info_t));
    for(unsigned i = 0; i < cBatches; ++i)
    {
        V3D_DBGPRINT("Batch %u: vert %u tri %u bone_links_size %X\n", i, Batches[i].vertices_count, Batches[i].triangles_count, Batches[i].bone_links_size);
        //m_pMeshMgr->GetGame()->GetConsole()->DbgPrint("wtf %d tri %d ver %d\n", Batches[i].bone_links_size, Batches[i].triangles_count, Batches[i].vertices_count);
        ASSERT(Batches[i].vertices_count < 1000);
        ASSERT(Batches[i].triangles_count < 1000);
        ASSERT(Batches[i].positions_size == V3D_ALIGN_UP(Batches[i].vertices_count * 3 * sizeof(float)));
        ASSERT(Batches[i].indices_size == Batches[i].triangles_count * 4 * sizeof(uint16_t));
        ASSERT(Batches[i].unknown_size <= 0x1000); // usually near vertices_count*2
        ASSERT(Batches[i].bone_links_size == V3D_ALIGN_UP(Batches[i].vertices_count * 8));
        ASSERT(Batches[i].tex_coords_size == V3D_ALIGN_UP(Batches[i].vertices_count * 2 * sizeof(float)));
        ASSERT(Batches[i].unknown3 == 0x518C41 || Batches[i].unknown3 == 0x110C21);
        cVerifyTotalVertices += Batches[i].vertices_count;
        cVerifyTotalTriangles += Batches[i].triangles_count;
    }
    V3D_DBGPRINT("wtf lod unk0 %d totalvert %d totaltri %d\n", Unknown0, cVerifyTotalVertices, cVerifyTotalTriangles);
    //ASSERT(cVerifyTotalVertices == cLodVertices);
    
    uint32_t Unknown2 = Stream.ReadUInt32(); // unknown2 (usually 0, 1)
    ASSERT(Unknown2 <= 32);
    
    uint32_t cTextures = Stream.ReadUInt32();
    ASSERT(cTextures >= cBatches); // usually equal
    if (cTextures != cBatches)
        V3D_DBGPRINT("  >>> cTextures %u cBatches %u\n", cTextures, cBatches);
        
    //V3D_DBGPRINT("Textures %u at 0x%x\n", cTextures, Stream.tellg());
    //ASSERT(cTextures <= cBatches);
    //if(bIrrMesh)
    //    assert(m_Materials.empty()); // FIXME
    for(unsigned i = 0; i < cTextures; ++i)
    {
        //V3D_DBGPRINT("Texture %u at 0x%x\n", i, Stream.tellg());
        
        uint8_t Id = Stream.ReadUInt8();
        //ASSERT(Id <= cBatches);
        //ASSERT(Id == i);
        CString strFilename = Stream.ReadString();
        ASSERT(strFilename.length() >= 5 && strFilename.length() < 32);
        //if (Id >= cBatches)
        V3D_DBGPRINT("Batch texture: ID 0x%x name %s batches %d\n", Id, strFilename.c_str(), cBatches);
    }
    
    unsigned uOffset = ALIGN(cBatches * 56, V3D_ALIGNMENT);
        
    for(unsigned i = 0; i < cBatches; ++i)
    {
        ASSERT(uOffset < cbData);
            
        //V3D_DBGPRINT("Batch %u: vertices %u triangles %u possize 0x%x idxsize 0x%x, texcsize 0x%x\n", i,
        //                             Batches[i].vertices_count, Batches[i].triangles_count, Batches[i].positions_size, Batches[i].indices_size, Batches[i].tex_coords_size);
            
        //V3D_DBGPRINT("Positions offset 0x%x\n", uDataOffset + uOffset);
        float *pPositions = reinterpret_cast<float*>(pData + uOffset);
        uOffset = ALIGN(uOffset + Batches[i].positions_size, V3D_ALIGNMENT);
            
        //V3D_DBGPRINT("Normals offset 0x%x\n", uDataOffset + uOffset);
        float *pNormals = reinterpret_cast<float*>(pData + uOffset);
        uOffset = ALIGN(uOffset + Batches[i].positions_size, V3D_ALIGNMENT);
            
        //V3D_DBGPRINT("Tex coords offset 0x%x\n", uDataOffset + uOffset);
        float *pTexCoords = reinterpret_cast<float*>(pData + uOffset);
        uOffset = ALIGN(uOffset + Batches[i].tex_coords_size, V3D_ALIGNMENT);
            
        //V3D_DBGPRINT("Indices offset 0x%x\n", uDataOffset + uOffset);
        uint16_t *pIndices = reinterpret_cast<uint16_t*>(pData + uOffset);
        uOffset = ALIGN(uOffset + Batches[i].indices_size, V3D_ALIGNMENT);
            
        if (uFlags & 0x20)
        {
            uOffset = ALIGN(uOffset + Batches[i].triangles_count * 4 * sizeof(float), V3D_ALIGNMENT);
            
        } else if (!m_bIsCharacter) system("pause");
        
        //V3D_DBGPRINT("Unknown offset 0x%x\n", uDataOffset + uOffset);
        uOffset = ALIGN(uOffset + Batches[i].unknown_size, V3D_ALIGNMENT);
            
        //V3D_DBGPRINT("Bone links offset 0x%x\n", uDataOffset + uOffset);
        uOffset = ALIGN(uOffset + Batches[i].bone_links_size, V3D_ALIGNMENT);
            
        //V3D_DBGPRINT("Unknown3 offset 0x%x\n", uDataOffset + uOffset);
        if (uFlags & 0x1)
            uOffset = ALIGN(uOffset + Unknown0 * 2, V3D_ALIGNMENT);

        for(unsigned j = 0; j < Batches[i].triangles_count; ++j)
        {
            uint16_t idx1 = pIndices[j * 4 + 0], idx2 = pIndices[j * 4 + 1], idx3 = pIndices[j * 4 + 2];
            uint16_t idx4 = pIndices[j * 4 + 3];
                
            if(idx1 >= Batches[i].vertices_count || idx2 >= Batches[i].vertices_count || idx3 >= Batches[i].vertices_count)
                V3D_DBGPRINT("Invalid triangle %u %u %u\n", idx1, idx2, idx3);
                
            ASSERT(idx1 < Batches[i].vertices_count);
            ASSERT(idx2 < Batches[i].vertices_count);
            ASSERT(idx3 < Batches[i].vertices_count);
            ASSERT(idx4 == 0 || idx4 == 0x20);
            
            btVector3 vPt1(pPositions[idx1 * 3 + 0], pPositions[idx1 * 3 + 1], pPositions[idx1 * 3 + 2]);
            btVector3 vPt2(pPositions[idx2 * 3 + 0], pPositions[idx2 * 3 + 1], pPositions[idx2 * 3 + 2]);
            btVector3 vPt3(pPositions[idx3 * 3 + 0], pPositions[idx3 * 3 + 1], pPositions[idx3 * 3 + 2]);
            ASSERT(isPosVector(vPt1) && isPosVector(vPt2) && isPosVector(vPt3));
        }
    }

    uOffset += Unknown2 * 0x64; // v3d_lod_unk_effect_t
    ASSERT(uOffset == cbData);
    
    delete[] pData;
    
    ASSERT(Stream);
}
