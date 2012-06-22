/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CEvent.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifdef OF_SERVER
# include "CServer.h"
#endif // OF_SERVER
#include "utils.h"
#include "CEvent.h"
#include "CLogger.h"
#include "CLevel.h"
#include "CEntity.h"
#include "CElementsMgr.h"

CEvent::CEvent(CLevel *pLevel, CInputBinaryStream &Stream):
    CElement(OFET_EVENT, pLevel),
    m_nWait(0)
{
    m_nUid = Stream.ReadUInt32();
    m_strClass = Stream.ReadString2();
    //m_pLevel->GetGame()->GetConsole()->PrintStatus("Loading event: %u (%s)\n", m_nUid, m_strClass.c_str());
    
    Stream.ignore(12); // pos
    Stream.ReadString2(); // script name
    Stream.ignore(1); // unknown
    m_nDelay = (unsigned)(Stream.ReadFloat() * 1000);
    
    m_Bool1 = Stream.ReadUInt8() ? true : false;
    m_Bool2 = Stream.ReadUInt8() ? true : false;
    m_Int1 = Stream.ReadInt32();
    m_Int2 = Stream.ReadInt32();
    m_Float1 = Stream.ReadFloat();
    m_Float2 = Stream.ReadFloat();
    m_String1 = Stream.ReadString2();
    m_String2 = Stream.ReadString2();
    
    unsigned cLinks = Stream.ReadUInt32();
    if(cLinks > 128)
        CLogger::GetInst().PrintError("Warning! Trigger can be corruptted: %u links\n", cLinks);
    m_Links.reserve(cLinks);
    for(unsigned i = 0; i < cLinks; ++i)
        m_Links.push_back(Stream.ReadUInt32());
    
    if(!StrCmpI(m_strClass.c_str(), "Alarm") || !StrCmpI(m_strClass.c_str(), "Teleport") ||
       !StrCmpI(m_strClass.c_str(), "Play_Vclip") || !StrCmpI(m_strClass.c_str(), "Teleport_Player"))
    {
        Stream.ignore(9 * sizeof(float)); // rotation matrix
    }
    
    Stream.ignore(4); // color
}

bool CEvent::Activate(CEntity *pEntity)
{
    if(m_nWait)
        return false;
    
    m_pEntity = pEntity;
    if(m_nDelay == 0)
        Execute();
    else
        m_nWait = m_nDelay;
    
    return true;
}

void CEvent::Update(unsigned nDeltaTime)
{
    if(!m_nWait)
        return; // not activated
    
    if(nDeltaTime < m_nWait)
        m_nWait -= nDeltaTime;
    else
    {
        m_nWait = 0;
        Execute();
    }
}

void CEvent::Execute()
{
    if(!StrCmpI(m_strClass.c_str(), "Continuous_Damage"))
    {
        if(m_pEntity && m_pEntity->IsAlive())
        {
            SDamageInfo DmgInfo;
            DmgInfo.pResponsibleObj = m_pEntity;
            
            if(m_Int1)
                m_pEntity->Damage((float)m_Int1, DmgInfo);
            else
                m_pEntity->Kill(DmgInfo);
        }
    }
#ifdef OF_SERVER // FIXME
    else if(!StrCmpI(m_strClass.c_str(), "Load_Level"))
    {
        if(m_String1.size() < 4 || StrCmpI(m_String1.c_str() + m_String1.size() - 4, ".rfl") != 0)
            m_String1 += ".rfl";
        
        //if(StrCmpI(CServer::GetInst().GetLevelFileName(), m_String1.c_str()) != 0)
        //    CServer::GetInst().QueueLevelChange(m_String1.c_str());
    }
#endif // OF_SERVER
    
    /* Activate linked events */
    for(unsigned i = 0; i < m_Links.size(); ++i)
    {
        CElement *pElement = m_pLevel->GetElementsMgr()->Get(m_Links[i]);
        if(pElement && pElement->GetType() == OFET_EVENT)
        {
            CEvent * const pEvent = (CEvent*)pElement;
            pEvent->Activate(m_pEntity);
        }
    }
}
