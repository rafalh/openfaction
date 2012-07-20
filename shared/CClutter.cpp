/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CClutter.cpp
*  PURPOSE:     Clutter element
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CClutter.h"
#include "CColWorld.h"
#include "CClutterTable.h"
#include "CMesh.h"
#include "CMeshMgr.h"
#include "CLevel.h"
#include "CGame.h"
#include "CEventsHandler.h"

using namespace std;
#ifdef OF_CLIENT
using namespace irr;
#endif // OF_CLIENT

btSphereShape CClutter::m_ColShape(0.6f);

CClutter::CClutter(CLevel *pLevel, CInputBinaryStream &Stream):
    CKillableObject(OFET_CLUTTER, pLevel)
{
    m_iColGroup = COL_CLUTTER;
    m_iColMask = COL_NONE;
    
    // create collision object with deafult shape
    m_pColObj = new btRigidBody(0.0f, &m_MotionState, &m_ColShape);
    m_pColObj->setUserPointer(this);
    
    m_nUid = Stream.ReadUInt32();
    
    string strClassName = Stream.ReadString2();
    m_pClass = m_pLevel->GetGame()->GetClutterTbl()->Get(strClassName.c_str());
    
    SetPos(Stream.ReadVector());
    
    btMatrix3x3 matRot;
    matRot[2] = Stream.ReadVector();
    matRot[0] = Stream.ReadVector();
    matRot[1] = Stream.ReadVector();
    SetRotMatrix(matRot.transpose());
    
    Stream.ReadString2(); // script name
    Stream.ignore(5); // unknown
    Stream.ReadString2(); // skin
    
    unsigned cLinks = Stream.ReadUInt32();
    Stream.ignore(cLinks * 4);
    
    m_fLife = m_pClass ? m_pClass->fHealth : -1.0f;
    
    m_pMesh = m_pClass ? m_pLevel->GetGame()->GetMeshMgr()->Load(m_pClass->strMeshFilename) : NULL;
    if(m_pMesh)
    {
        assert(m_pMesh->GetSubMeshCount() > 0);
        
        btCollisionShape *pColShape = m_pMesh->GetSubMesh(0)->GetColShape();
        m_pColObj->setCollisionShape(pColShape);
        
#ifdef OF_CLIENT
        scene::IMesh *pIrrMesh = m_pMesh->GetSubMesh(0)->GetIrrMesh();
        assert(pIrrMesh);
        m_pSceneNode = m_pLevel->GetGame()->GetSceneMgr()->addMeshSceneNode(pIrrMesh);
#endif // OF_CLIENT
    }
        
    AddToWorld();
}

CClutter::~CClutter()
{
    if(m_pMesh)
        m_pMesh->Release();
    m_pMesh = NULL;
}

void CClutter::Kill(SDamageInfo &DmgInfo)
{
    if(!IsAlive())
        return; // already dead
    
    m_fLife = 0.0f;
    
    m_pLevel->GetGame()->GetEventsHandler()->OnClutterKill(this, DmgInfo);
    RemoveFromWorld();
}
