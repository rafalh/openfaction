/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CStaticGeometry.cpp
*  PURPOSE:     Loading of static geometry in levels
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CStaticGeometry.h"
#include "CLevel.h"
#include "CLevelProperties.h"
#include "CConsole.h"
#include "formats/rfl_format.h"
#include "CGame.h"
#include "CEventsHandler.h"
#ifdef OF_CLIENT
# include "CTextureMgr.h"
# include "CLightmaps.h"
# include "irr/CReadFile.h"
# include "irr/CMeshSkyboxSceneNode.h"
# include "irr/CVbmAnimator.h"
#endif // OF_CLIENT
#include <util/utils.h>
#include <cmath>
#include <cassert>

using namespace std;
#ifdef OF_CLIENT
using namespace irr;
#endif // OF_CLIENT

struct SIndexDesc
{
    unsigned idx;
    float u, v;
    float lm_u, lm_v;
};

struct SFace
{
    btVector3 vNormal;
    unsigned iTexture;
    unsigned iLightmapUnk;
    unsigned uFlags;
    unsigned iRoom;
    unsigned uPortalUnk;
    vector<SIndexDesc> Indices;
};

#ifdef OF_CLIENT
struct SIrrRoom
{
    SIrrRoom(): pIrrMesh(NULL) {}
    
    scene::SMesh *pIrrMesh;
    vector<scene::ISceneNodeAnimator*> Animators;
};
#endif

CRoom::CRoom(CLevel *pLevel, unsigned nId, float fLife, bool bDetail):
    CKillableObject(OFET_LEVEL, pLevel, nId),
    m_pMesh(NULL), m_pShape(NULL),
    m_bDetail(bDetail)
{
    m_iColGroup = COL_LEVEL;
    m_iColMask = COL_ENTITY;
    
    SetLife(fLife);
}

CRoom::~CRoom()
{
    if(m_pShape)
    {
        assert(m_pColObj);
        m_pColObj->setCollisionShape(NULL);
        delete m_pShape;
        m_pShape = NULL;
    }
    if(m_pMesh)
        delete m_pMesh;
    m_pMesh = NULL;
}

void CRoom::Kill(SDamageInfo &DmgInfo)
{
    if(!IsAlive())
        return; // already dead
    
    m_fLife = 0.0f;
    
    m_pLevel->GetGame()->GetEventsHandler()->OnGlassKill(this, DmgInfo);
    RemoveFromWorld();
}

void CStaticGeometry::Load(CLevel *pLevel, CInputBinaryStream &Stream, unsigned nVersion)
{
    /* Unload old geometry first */
    Unload();
    
    Stream.ignore(nVersion == 0xB4 ? 6 : 10); // unknown
    
    unsigned cTextures = Stream.ReadUInt32();
    assert(m_Materials.empty());

    std::vector<CString> MaterialNames;

    for(unsigned i = 0; i < cTextures; ++i)
    {
        CString strFilename = Stream.ReadString2();
        MaterialNames.push_back(strFilename);
#ifdef OF_CLIENT
        CMultiTexture *pMaterial = pLevel->GetGame()->GetTextureMgr()->Load(strFilename);
        m_Materials.push_back(pMaterial);
#endif // OF_CLIENT
    }
    
    unsigned cScrollAnim = Stream.ReadUInt32();
    Stream.ignore(cScrollAnim * 12);
    
    unsigned cRooms = Stream.ReadUInt32();
    m_Rooms.reserve(cRooms);
#ifdef OF_CLIENT
    std::vector<SIrrRoom> IrrRooms(cRooms);
#endif // OF_CLIENT
    
    unsigned iGlass = 0;
    
    for(unsigned i = 0; i < cRooms; ++i)
    {
        unsigned nId = Stream.ReadUInt32();
        
        btVector3 vAabb1 = Stream.ReadVector();
        btVector3 vAabb2 = Stream.ReadVector();
        assert(vAabb1.x() < vAabb2.x() && vAabb1.y() < vAabb2.y() && vAabb1.z() < vAabb2.z());
        
        bool bSkyRoom = Stream.ReadUInt8() ? true : false;
        Stream.ignore(3); // is_cold, is_outside, is_airlock
        bool bLiquidRoom = Stream.ReadUInt8() ? true : false;
        bool bAmbientLight = Stream.ReadUInt8() ? true : false;
        bool bDetail = Stream.ReadUInt8() ? true : false;
        
        unsigned Unknown = Stream.ReadUInt8();
        assert(Unknown <= 1);
        
        float fLife = Stream.ReadFloat();
        assert(fLife >= -1.0f && fLife <= 10000.f);
        
        Stream.ReadString2(); // eax_effect
        
        if(bLiquidRoom)
        {
            Stream.ignore(8); // liquid_depth, liquid_color
            Stream.ReadString2(); // liquid_surface_texture
            Stream.ignore(12 + 13 + 12); // liquid_visibility, liquid_type, liquid_alpha, liquid_unknown,
                                         // liquid_waveform, liquid_surface_texture_scroll_u, liquid_surface_texture_scroll_b
        }
        
        if(bAmbientLight)
            Stream.ignore(4); // ambient_color
        
        CRoom *pRoom = new CRoom(pLevel, nId, fLife, bDetail);
        pRoom->m_GlassIndex = pRoom->IsGlass() ? (iGlass++) : 0;
        pRoom->m_bSkyRoom = bSkyRoom;
        pRoom->m_vCenter = btVector3((vAabb1.x() + vAabb2.x())/2.0f, (vAabb1.y() + vAabb2.y())/2.0f, (vAabb1.z() + vAabb2.z())/2.0f);
        m_Rooms.push_back(pRoom);
    }
    
    unsigned cUnknown = Stream.ReadUInt32();
    if(cRooms != cUnknown)
        pLevel->GetGame()->GetConsole()->DbgPrint("Warning! cRooms(%u) != cUnknown(%u)\n", cRooms, cUnknown);
    for(unsigned i = 0; i < cUnknown; ++i)
    {
        Stream.ignore(4); // index
        unsigned cUnknown2 = Stream.ReadUInt32(); // links_count
        Stream.ignore(cUnknown2 * 4); // links
    }
    
    unsigned cUnknown2 = Stream.ReadUInt32();
    Stream.ignore(cUnknown2 * 32); // unknown3
    
    unsigned cVertices = Stream.ReadUInt32();
    std::vector<btVector3> Vertices(cVertices);
    for(unsigned i = 0; i < cVertices; ++i)
        Vertices[i] = Stream.ReadVector();
    
    unsigned cFaces = Stream.ReadUInt32();
	std::vector<SFace> Faces(cFaces);
    
    //CGame::GetInst().GetSceneMgr()->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);
    for(unsigned i = 0; i < cFaces; ++i)
    {
        unsigned nPos = (unsigned)Stream.tellg();
        SFace &Face = Faces[i];
        
        // Use normal from unknown plane
        Face.vNormal = Stream.ReadVector();
        float fLen2 = Face.vNormal.length2();
        assert(fLen2 > 0.99f && fLen2 < 1.01f);
        float fDist = Stream.ReadFloat();
        //assert(fDist >= 0.0f);
        
        Face.iTexture = Stream.ReadUInt32();
        if(Face.iTexture != 0xFFFFFFFF)
            assert(Face.iTexture < cTextures);
        
        Face.iLightmapUnk = Stream.ReadUInt32(); // its used later (lightmap?)
        assert(Face.iLightmapUnk == 0xFFFFFFFF || Face.iLightmapUnk < cFaces);
        
        unsigned Unknown3 = Stream.ReadUInt32();
        //assert(Unknown3 == i);
        
        unsigned Unknown4 = Stream.ReadUInt32();
        assert(Unknown4 == 0xFFFFFFFF);
        
        unsigned Unknown4_2 = Stream.ReadUInt32();
        assert(Unknown4_2 == 0xFFFFFFFF);
        
        Face.uPortalUnk = Stream.ReadUInt32(); // its used later (it's not 0 for portals)
        
        Face.uFlags = Stream.ReadUInt8();
        if((Face.uFlags & ~(RFL_FF_MASK)))
            pLevel->GetGame()->GetConsole()->DbgPrint("Unknown face flags 0x%x\n", Face.uFlags & ~(RFL_FF_MASK));
        
        uint8_t uLightmapRes = Stream.ReadUInt8();
        if(uLightmapRes >= 0x22)
            pLevel->GetGame()->GetConsole()->DbgPrint("Unknown lightmap resolution 0x%x\n", uLightmapRes);
        
        uint16_t Unk6 = Stream.ReadUInt16();
        uint32_t Unk6_2 = Stream.ReadUInt32();
        assert(Unk6 == 0);
        
        Face.iRoom = Stream.ReadUInt32();
        assert(Face.iRoom < cRooms);
        
        unsigned cFaceVertices = Stream.ReadUInt32();
        assert(cFaceVertices >= 3);
        //pLevel->GetGame()->GetConsole()->DbgPrint("Face %u vertices: %u, texture: %x (pos: 0x%x)\n", i, cFaceVertices, iTexture, nPos);
        if(cFaceVertices > 100)
            pLevel->GetGame()->GetConsole()->DbgPrint("Warning! Face %u has %u vertices (level can be corrupted) (pos: 0x%x)\n", i, cFaceVertices, nPos);
        
        Face.Indices.reserve(cFaceVertices);
        
        for(unsigned j = 0; j < cFaceVertices; ++j)
        {
            SIndexDesc Idx;
            
            Idx.idx = Stream.ReadUInt32();
            assert(Idx.idx < cVertices);
            
            Idx.u = Stream.ReadFloat();
            Idx.v = Stream.ReadFloat();
            if(Face.iLightmapUnk != 0xFFFFFFFF)
            {
                Idx.lm_u = Stream.ReadFloat();
                Idx.lm_v = Stream.ReadFloat();
                assert(Idx.lm_u >= 0.0f && Idx.lm_u <= 1.0f);
                assert(Idx.lm_v >= 0.0f && Idx.lm_v <= 1.0f);
            }
            
            Face.Indices.push_back(Idx);
        }
    }
    
    unsigned cLightmapVertices = Stream.ReadUInt32();
#ifdef OF_CLIENT
    std::vector<unsigned> LightmapVertices(cLightmapVertices);
#endif // OF_CLIENT

    for(unsigned i = 0; i < cLightmapVertices; ++i)
    {
        unsigned iLightmap = Stream.ReadUInt32();
#ifdef OF_CLIENT
        assert(iLightmap < pLevel->GetLightmaps()->GetCount());
        LightmapVertices[i] = iLightmap;
#endif // OF_CLIENT
        Stream.ignore(92); // unknown4
    }
    
    if(nVersion == 0xB4)
        Stream.ignore(4); // unknown5
    
    for(unsigned i = 0; i < cFaces; ++i)
    {
        SFace &Face = Faces[i];
        CRoom *pRoom = m_Rooms[Face.iRoom];
        CMultiTexture *pMaterial = (Face.iTexture != 0xFFFFFFFF) ? m_Materials[Face.iTexture] : NULL;
        const CString &MaterialName = (Face.iTexture != 0xFFFFFFFF) ? MaterialNames[Face.iTexture] : "";
        bool bIsTransparent = (Face.uPortalUnk != 0) || (Face.uFlags & RFL_FF_SHOW_SKY) || IsInvisibleLevelTexture(MaterialName);
        
#ifdef OF_CLIENT
        video::ITexture *pTexture = NULL, *pLightmapTexture = NULL;
        scene::SMeshBufferLightMap *pIrrBuf;
        unsigned iBuf, nFirstSubmeshIndex;
        
        if(!bIsTransparent)
        {
            // Get texture pointer
            if(pMaterial)
                pTexture = pMaterial->GetFrame(0);
            
            if(Face.iLightmapUnk != 0xFFFFFFFF)
            {
                unsigned iLightmap = LightmapVertices[Face.iLightmapUnk];
                pLightmapTexture = pLevel->GetLightmaps()->Get(iLightmap);
            }
            
            if(!IrrRooms[Face.iRoom].pIrrMesh)
            {
                IrrRooms[Face.iRoom].pIrrMesh = new scene::SMesh;
                assert(IrrRooms[Face.iRoom].pIrrMesh->getMeshBufferCount() == 0);
            }
            
            // Find buffer for specified texture
            for(iBuf = 0; iBuf < IrrRooms[Face.iRoom].pIrrMesh->getMeshBufferCount(); ++iBuf)
            {
                pIrrBuf = (scene::SMeshBufferLightMap*)IrrRooms[Face.iRoom].pIrrMesh->getMeshBuffer(iBuf);
                if(pIrrBuf->Material.getTexture(0) == pTexture &&
                   pIrrBuf->Material.getTexture(1) == pLightmapTexture)
                {
                    break;
                }
            }
            
            if(iBuf == IrrRooms[Face.iRoom].pIrrMesh->getMeshBufferCount())
            {
                // Create new buffer and prepare it
                pIrrBuf = new scene::SMeshBufferLightMap;
                pIrrBuf->setHardwareMappingHint(scene::EHM_STATIC);
                
                // Setup material flags
                pIrrBuf->Material.setFlag(video::EMF_FOG_ENABLE, pLevel->GetProperties()->IsFogEnabled());
                pIrrBuf->Material.setFlag(video::EMF_LIGHTING, false);
                
                // Setup textures
                pIrrBuf->Material.setTexture(0, pTexture);
                pIrrBuf->Material.setTexture(1, pLightmapTexture);
                
                if(pRoom->IsDetail() && pMaterial && pMaterial->HasAlpha())
                    pIrrBuf->Material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
                else if(pLightmapTexture && !pRoom->m_bSkyRoom)
                    pIrrBuf->Material.MaterialType = video::EMT_LIGHTMAP_M2;
                
                if(pMaterial && pMaterial->IsAnimated())
                {
                    scene::ISceneNodeAnimator *pAnim = pMaterial->CreateAnimator(iBuf);
                    if(pAnim)
                        IrrRooms[Face.iRoom].Animators.push_back(pAnim);
                }
                
                // Add buffer to the mesh
                IrrRooms[Face.iRoom].pIrrMesh->addMeshBuffer(pIrrBuf);
                pIrrBuf->drop();
            }
            
            // Reallocate buffers to speed up things
            pIrrBuf->Vertices.reallocate(pIrrBuf->Vertices.size() + Face.Indices.size());
            pIrrBuf->Indices.reallocate(pIrrBuf->Indices.size() + (Face.Indices.size() - 3 + 1) * 3);
            
            // First index points to first vertex
            nFirstSubmeshIndex = pIrrBuf->Vertices.size();
        }
#endif // OF_CLIENT
        
        unsigned nFirstIndex, nPrevIndex;
        
        for(unsigned j = 0; j < Face.Indices.size(); ++j)
        {
            SIndexDesc &Idx = Face.Indices[j];
            
            if(!bIsTransparent)
            {
                if(!pRoom->m_bSkyRoom)
                {
                    if(j == 0)
                        nFirstIndex = Idx.idx;
                    else if(j >= 2)
                    {
                        if(!pRoom->m_pMesh)
                        {
                            pRoom->m_pMesh = new btTriangleMesh();
                            pRoom->m_pMesh->preallocateVertices(Face.Indices.size());
                            pRoom->m_pMesh->preallocateIndices((Face.Indices.size() - 3 + 1) * 3);
                        }
                        
                        pRoom->m_pMesh->addTriangle(Vertices[nFirstIndex] - pRoom->m_vCenter, Vertices[nPrevIndex] - pRoom->m_vCenter, Vertices[Idx.idx] - pRoom->m_vCenter);
                    }
                }
                
#ifdef OF_CLIENT
                btVector3 vPos = Vertices[Idx.idx] - pRoom->m_vCenter;
                video::S3DVertex2TCoords Vertex;
                Vertex.Pos.X = vPos.x();
                Vertex.Pos.Y = vPos.y();
                Vertex.Pos.Z = vPos.z();
                Vertex.Normal.X = Face.vNormal.x();
                Vertex.Normal.Y = Face.vNormal.y();
                Vertex.Normal.Z = Face.vNormal.z();
                Vertex.TCoords.X = Idx.u;
                Vertex.TCoords.Y = Idx.v;
                Vertex.TCoords2.X = Idx.lm_u;
                Vertex.TCoords2.Y = Idx.lm_v;
                Vertex.Color = video::SColor(255, 255, 255, 255);
                
                pIrrBuf->Vertices.push_back(Vertex);
                
                if(j >= 2)
                {
                    pIrrBuf->Indices.push_back(nFirstSubmeshIndex);
                    pIrrBuf->Indices.push_back(pIrrBuf->Vertices.size() - 2);
                    pIrrBuf->Indices.push_back(pIrrBuf->Vertices.size() - 1);
                }
#endif // OF_CLIENT
            }
            
            nPrevIndex = Idx.idx;
        }
    }
    
    for(unsigned i = 0; i < m_Rooms.size(); ++i)
    {
        CRoom *pRoom = m_Rooms[i];
        
        if(pRoom->m_pMesh)
        {
            pRoom->m_pShape = new btBvhTriangleMeshShape(pRoom->m_pMesh, true);
            btRigidBody::btRigidBodyConstructionInfo ConstructionInfo(0.0f, &pRoom->m_MotionState, pRoom->m_pShape);
            ConstructionInfo.m_friction = 10.0f;
            ConstructionInfo.m_restitution = 1.0f;
            pRoom->m_pColObj = new btRigidBody(ConstructionInfo);
            pRoom->m_pColObj->setUserPointer(pRoom);
            pRoom->m_pColObj->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
            pRoom->m_pColObj->getWorldTransform().setOrigin(pRoom->m_vCenter);
            
            pRoom->AddToWorld();
        }
        
#ifdef OF_CLIENT
        if(IrrRooms[i].pIrrMesh)
        {
            // Update bounding boxes
            for(unsigned iBuf = 0; iBuf < IrrRooms[i].pIrrMesh->getMeshBufferCount(); ++iBuf)
                IrrRooms[i].pIrrMesh->getMeshBuffer(iBuf)->recalculateBoundingBox();
            IrrRooms[i].pIrrMesh->recalculateBoundingBox();
            
            // Create scene node
            if(pRoom->m_bSkyRoom)
            {
                scene::ISceneManager *pSM = pLevel->GetGame()->GetSceneMgr();
                pRoom->m_pSceneNode = new scene::CMeshSkyboxSceneNode(IrrRooms[i].pIrrMesh, pSM->getRootSceneNode(), pSM, -1);
                pRoom->m_pSceneNode->drop(); // drop it because smgr owns it now
            }
            else
            {
                pRoom->m_pSceneNode = pLevel->GetGame()->GetSceneMgr()->addOctreeSceneNode(IrrRooms[i].pIrrMesh);
                pRoom->m_pSceneNode->setPosition(core::vector3df(pRoom->m_vCenter.x(), pRoom->m_vCenter.y(), pRoom->m_vCenter.z()));
            }
            
            // Setup animators
            for(unsigned iAnim = 0; iAnim < IrrRooms[i].Animators.size(); ++iAnim)
            {
                pRoom->m_pSceneNode->addAnimator(IrrRooms[i].Animators[iAnim]);
                IrrRooms[i].Animators[iAnim]->drop(); // Scene node owns animators
            }
            IrrRooms[i].Animators.clear();
        } else
            assert(IrrRooms[i].Animators.empty());
#endif // OF_CLIENT
    }
    
    pLevel->GetGame()->GetConsole()->DbgPrint("Loaded geometry: %u textures, %u rooms, %u vertices, %u faces\n", cTextures, cRooms, cVertices, cFaces);
}

void CStaticGeometry::Unload()
{
    // Cleanup rooms - they are deleted by CLevel
    m_Rooms.clear();
    
#ifdef OF_CLIENT
    // Cleanup materials
    for(unsigned i = 0; i < m_Materials.size(); ++i)
        m_Materials[i]->Release();
    m_Materials.clear();
#endif // OF_CLIENT
}
