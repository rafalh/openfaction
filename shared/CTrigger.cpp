/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CTrigger.cpp
*  PURPOSE:     Trigger element
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifdef OF_SERVER
# include "../server/CServer.h"
#endif
#include "CTrigger.h"
#include "CEvent.h"
#include "util/CLogger.h"
#include "CLevel.h"
#include "CElementsMgr.h"

CTrigger::CTrigger(CLevel *pLevel, CInputBinaryStream &Stream):
    CObject(OFET_TRIGGER, pLevel), m_nWait(0)
{
    m_iColGroup = COL_TRIGGER;
    m_iColMask = (short)COL_NONE;
    
    m_pColObj = new btCollisionObject();
    m_pColObj->setUserPointer(this);
    m_pColObj->setCollisionFlags(m_pColObj->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    
    m_nUid = Stream.ReadUInt32();
    
    Stream.ReadString2(); // script name
    Stream.ignore(1); // unknown
    bool bBox = Stream.ReadUInt8() ? true : false;
    Stream.ignore(3); // unknown2
    SetResetTime((unsigned)(Stream.ReadFloat() * 1000));
    SetResetsCount(Stream.ReadUInt16()); // 0xFFFF - infinity
    Stream.ignore(2); // unknown3
    SetUseKeyRequired(Stream.ReadUInt8() ? true : false);
    Stream.ReadString2(); // key name
    bool bWeaponActivates = Stream.ReadUInt8() ? true : false;
    Stream.ignore(1); // unknown4
    bool bNpc = Stream.ReadUInt8() ? true : false;
    m_bAuto = Stream.ReadUInt8() ? true : false;
    bool bVehicleActivates = Stream.ReadUInt8() ? true : false;
    SetPos(Stream.ReadVector());
    if(bBox)
    {
        btMatrix3x3 RotMatrix;
        RotMatrix[2] = Stream.ReadVector();
        RotMatrix[0] = Stream.ReadVector();
        RotMatrix[1] = Stream.ReadVector();
        SetRotMatrix(RotMatrix);
        btVector3 Size;// = Stream.ReadVector();
        Size.setY(Stream.ReadFloat());
        Size.setX(Stream.ReadFloat());
        Size.setZ(Stream.ReadFloat());
        SetBoxShape(Size);
        bool bOneWay = Stream.ReadUInt8() ? true : false;
    }
    else
        SetSphereShape(Stream.ReadFloat());
    
    unsigned nAirlockRoomUid = Stream.ReadUInt32();
    unsigned nAttachedToUid = Stream.ReadUInt32();
    unsigned nUseClutterUid = Stream.ReadUInt32();
    m_bEnabled = Stream.ReadUInt8() ? false : true; // is disabled
    float fButtonActiveTime = Stream.ReadFloat();
    float fInsideTime = Stream.ReadFloat();
    Stream.ignore(4); // unknown5
    
    unsigned cLinks = Stream.ReadUInt32();
    if(cLinks > 128)
        CLogger::GetInst().PrintError("Warning! Trigger can be corruptted: %u links\n", cLinks);
    m_Links.reserve(cLinks);
    for(unsigned i = 0; i < cLinks; ++i)
        m_Links.push_back(Stream.ReadUInt32());
    
    AddToWorld();
}

bool CTrigger::Activate(CEntity *pEntity)
{
    if(m_nWait > 0 || !m_cResets)
        return false;
    
    m_nWait = m_nResetTime;
    if(m_cResets != 0xFFFFFFFF)
        --m_cResets;

#ifdef OF_SERVER
    for(unsigned i = 0; i < m_Links.size(); ++i)
    {
        CElement *pElement = m_pLevel->GetElementsMgr()->Get(m_Links[i]);
        CEvent *pEvent = dynamic_cast<CEvent*>(pElement);
        if(pEvent)
            pEvent->Activate(pEntity);
    }
#endif // OF_SERVER
    
    return true;
}

void CTrigger::Update(unsigned nDeltaTime)
{
    if(m_nWait > nDeltaTime)
        m_nWait -= nDeltaTime;
    else
        m_nWait = 0;
}
