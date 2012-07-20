/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CLevel.cpp
*  PURPOSE:     Level handling
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifdef OF_SERVER
# include "CServer.h"
# include "CSpawnpoint.h"
# include "CSpawnpointsManager.h"
# include "CPlayer.h"
#else // !OF_SERVER
# include "CLightmaps.h"
# include <irrlicht.h>
#endif // OF_SERVER
#include "rfl_format.h"
#include "utils.h"
#include "errors.h"
#include "CLevel.h"
#include "CLogger.h"
#include "CStringsTable.h"
#include "CEntitiesTable.h"
#include "CItemsTable.h"
#include "CColWorld.h"
#include "CElementsMgr.h"
#include "CLevelProperties.h"
#include "CConsole.h"
#include "CStaticGeometry.h"
#include "CException.h"
#include "CGame.h"

#include "CClutter.h"
#include "CEntity.h"
#include "CEvent.h"
#include "CItem.h"
#include "CTrigger.h"
#include "CProjectile.h"

using namespace std;

CLevel::CLevel(CGame *pGame):
    m_pGame(pGame),
    m_strName(""),
#ifdef OF_SERVER
    m_pSpawnpointsMgr(NULL),
#endif // OF_SERVER
#ifdef OF_CLIENT
    m_pLightmaps(NULL),
#endif // OF_CLIENT
    m_pStaticGeometry(NULL)
{
    m_pColWorld = new CColWorld(this);
    m_pElementsMgr = new CElementsMgr;
    m_pLvlProp = new CLevelProperties(this);
#ifdef OF_SERVER
    m_pSpawnpointsMgr = new CSpawnpointsManager(this);
#endif // OF_SERVER
}

CLevel::~CLevel()
{
    Unload();
    
    delete m_pColWorld;
    m_pColWorld = NULL;
    
    delete m_pElementsMgr;
    m_pElementsMgr = NULL;
    
    delete m_pLvlProp;
    m_pLvlProp = NULL;
    
#ifdef OF_SERVER
    delete m_pSpawnpointsMgr;
    m_pSpawnpointsMgr = NULL;
#endif // OF_SERVER
}

void CLevel::Load(CInputBinaryStream &Stream)
{
    // Unload previous level
    Unload();
    
    // Load header
    rfl_header_t Header;
    Stream.ReadBinary(&Header, sizeof(Header));
    if(!Stream.good())
        THROW_EXCEPTION("Failed to read level file header");
    
    if(Header.signature != RFL_SIGNATURE)
        THROW_EXCEPTION("Invalid level signature");
    
    if(ReadRflString(m_strName, Stream) < 0)
        THROW_EXCEPTION("Failed to read level name");
    
    if(strncmp(m_strName.c_str(), "XSTR(", 5) == 0)
        m_strName = m_pGame->GetStringsTbl()->ParseXStr(m_strName.c_str());
    
    IgnoreRflString(Stream); // mod
    
    m_pGame->GetConsole()->DbgPrint("Loading %s (version: 0x%X)\n", m_strName.c_str(), Header.version);
    
    while(true)
    {
        rfl_section_header_t SectionHeader;
        Stream.ReadBinary(&SectionHeader, sizeof(SectionHeader));
        if(!Stream.good() || SectionHeader.type == RFL_END)
            break;
        
        /* Save current position - we will need it */
        streampos nPos = Stream.tellg();
        
        //m_pGame->GetConsole()->DbgPrint("Section 0x%08X (size: 0x%08X), pos: 0x%08X\n", SectionHeader.type, SectionHeader.size, (unsigned)nPos);
        
        switch(SectionHeader.type)
        {
            case RFL_LEVEL_PROPERTIES:
                m_pLvlProp->Load(Stream);
                break;
            case RFL_CLUTTERS:
            {
                unsigned i, cClutter = Stream.ReadUInt32();
                for(i = 0; i < cClutter; ++i)
                {
                    if(!Stream.good())
                        break;
                    
                    try
                    {
                        CClutter *pClutter = new CClutter(this, Stream);
                    }
                    catch(const exception &e)
                    {
                        m_pGame->GetConsole()->Print("Warning! Failed to load clutter: %s\n", e.what());
                    }
                }
                m_pGame->GetConsole()->DbgPrint("Loaded %u/%u clutter\n", i, cClutter);
                break;
            }
            
            case RFL_ENTITIES:
            {
                unsigned i, cEntities = Stream.ReadUInt32();
                for(i = 0; i < cEntities; ++i)
                {
                    if(!Stream.good())
                        break;
                    
                    try
                    {
                        CEntity *pEntity = new CEntity(this, Stream);
                    }
                    catch(const exception &e)
                    {
                        m_pGame->GetConsole()->Print("Warning! Failed to load entity: %s\n", e.what());
                    }
                }
                m_pGame->GetConsole()->DbgPrint("Loaded %u/%u entities\n", i, cEntities);
                break;
            }
            
            case RFL_EVENTS:
            {
                unsigned i, cEvents = Stream.ReadUInt32();
                for(i = 0; i < cEvents; ++i)
                {
                    if(!Stream.good())
                        break;
                    
                    CEvent *pEvent = new CEvent(this, Stream);
                }
                m_pGame->GetConsole()->DbgPrint("Loaded %u/%u events\n", i, cEvents);
                break;
            }

            case RFL_ITEMS:
            {
                unsigned i, cItems = Stream.ReadUInt32();
                
                for(i = 0; i < cItems; ++i)
                {
                    if(!Stream.good())
                        break;
                    
                    CItem *pItem = new CItem(this, Stream, i);
                }
                
                m_pGame->GetConsole()->DbgPrint("Loaded %u/%u items\n", i, cItems);
                break;
            }
#ifdef OF_SERVER
            case RFL_MP_RESPAWNS:
            {
                unsigned i, cSpawnpoints = Stream.ReadUInt32();
                
                for(i = 0; i < cSpawnpoints; ++i)
                {
                    if(!Stream.good())
                        break;
                    
                    CSpawnpoint *pSpawnpoint = new CSpawnpoint(this, Stream);
                    m_pSpawnpointsMgr->Add(pSpawnpoint);
                }
                
                m_pGame->GetConsole()->DbgPrint("Loaded %u/%u spawnpoints\n", i, cSpawnpoints);
                break;
            }
#endif // OF_SERVER
            
            case RFL_PLAYER_START:
                m_vPlayerStartPos = Stream.ReadVector();
                m_PlayerStartRotMatrix[2] = Stream.ReadVector();
                m_PlayerStartRotMatrix[0] = Stream.ReadVector();
                m_PlayerStartRotMatrix[1] = Stream.ReadVector();
                m_pGame->GetConsole()->DbgPrint("Start position: %f %f %f\n", m_vPlayerStartPos.x(), m_vPlayerStartPos.y(), m_vPlayerStartPos.z());
                break;
            
            case RFL_TRIGGERS:
            {
                unsigned i, cTriggers = Stream.ReadUInt32();
                for(i = 0; i < cTriggers; ++i)
                {
                    if(!Stream.good())
                        break;
                    
                    CTrigger *pTrigger = new CTrigger(this, Stream);
                }
                m_pGame->GetConsole()->DbgPrint("Loaded %u/%u triggers\n", i, cTriggers);
                break;
            }
            
            case RFL_STATIC_GEOMETRY:
                assert(!m_pStaticGeometry);
                m_pStaticGeometry = new CStaticGeometry;
                m_pStaticGeometry->Load(this, Stream, Header.version);
                m_pGame->GetConsole()->DbgPrint("Loaded %u rooms\n", m_pStaticGeometry->GetRooms().size());
                break;
#ifdef OF_CLIENT
            case RFL_LIGHTMAPS:
            {
                assert(!m_pLightmaps);
                m_pLightmaps = new CLightmaps(m_pGame, Stream);
                m_pGame->GetConsole()->DbgPrint("Loaded %u lightmaps\n", m_pLightmaps->GetCount());
                break;
            }
#endif // OF_CLIENT
            default:
                //m_pGame->GetConsole()->DbgPrint("Unknown section 0x%08X (size: 0x%08X), pos: %d\n", SectionHeader.type, SectionHeader.size, (int)nPos);
                // Move to the next section
                Stream.seekg(nPos + (streampos)SectionHeader.size);
        }
        
        if(nPos + (streampos)SectionHeader.size != Stream.tellg())
        {
            // Move to the next section
            m_pGame->GetConsole()->DbgPrint("Warning! Section 0x%x is not completly processed...", SectionHeader.type);
            Stream.seekg(nPos + (streampos)SectionHeader.size);
        }
    }
    
#ifdef OF_SERVER
    /* If level has no multiplayer spawnpoints create new using player start position */
    if(m_pSpawnpointsMgr->GetRedCount() == 0 || m_pSpawnpointsMgr->GetBlueCount() == 0)
    {
        CSpawnpoint *pSpawnpoint = new CSpawnpoint(this);
        
        pSpawnpoint->SetPos(m_vPlayerStartPos);
        pSpawnpoint->SetRotMatrix(m_PlayerStartRotMatrix);
        pSpawnpoint->SetRedTeam(true);
        pSpawnpoint->SetBlueTeam(true);
        
        m_pSpawnpointsMgr->Add(pSpawnpoint);
        
        AddElement(pSpawnpoint);
        
        m_pGame->GetConsole()->DbgPrint("Added spawnpoint from player start section\n");
    }
    
    /* Calculate level checksum */
    Stream.clear(); // in case of error
    Stream.seekg(0);
    m_Checksum.Compute(Stream);
#endif // OF_SERVER
}

void CLevel::Unload()
{
#ifdef OF_SERVER
    // Remove spawnpoints
    m_pSpawnpointsMgr->Reset();
    
    // Fix players entities
    const set<CElement*> &Entities = m_pElementsMgr->GetByType(OFET_ENTITY);
    for(set<CElement*>::const_iterator it = Entities.begin(); it != Entities.end(); ++it)
    {
        CEntity *pEntity = (CEntity*)*it;
        if(pEntity->GetPlayer())
            pEntity->GetPlayer()->SetEntity(NULL);
    }
#endif // OF_SERVER
    
    // Delete all elements
    for(unsigned i = 0; i < OFET_MAX; ++i)
    {
        const set<CElement*> &Elements = m_pElementsMgr->GetByType((EElementType)i);
        while(!Elements.empty())
        {
            CElement * const pElement = *Elements.begin();
            delete pElement;
        }
    }

#ifdef OF_CLIENT
    // Unload lightmaps
    delete m_pLightmaps;
    m_pLightmaps = NULL;
#endif // OF_CLIENT
    
    // Unload geometry
    delete m_pStaticGeometry;
    m_pStaticGeometry = NULL;
}

CItem *CLevel::GetItemFromClass(const SItemClass *pClass)
{
    const set<CElement*> &Items = m_pElementsMgr->GetByType(OFET_ITEM);
    for(set<CElement*>::const_iterator it = Items.begin(); it != Items.end(); ++it)
    {
        CItem * const pItem = (CItem*)*it;
        
        if(pItem->GetClass() == pClass)
            return pItem;
    }
    
    return NULL;
}

void CLevel::Update(unsigned nDeltaTime)
{
    m_pColWorld->Update(nDeltaTime);
    
    set<CElement*>::const_iterator it;
    for(unsigned i = 0; i < OFET_MAX; ++i)
    {
        const set<CElement*> &Elements = m_pElementsMgr->GetByType((EElementType)i);
        for(it = Elements.begin(); it != Elements.end(); ++it)
            (*it)->Update(nDeltaTime);
    }
    
    while(!m_DeleteQueue.empty())
    {
        delete *(m_DeleteQueue.begin());
        m_DeleteQueue.erase(m_DeleteQueue.begin());
    }
}

void CLevel::DbgDraw() const
{
#if defined(DEBUG) && defined(OF_CLIENT)
    // Reset World matrix and draw collision world for debugging
    irr::video::IVideoDriver *pVideoDrv = m_pGame->GetVideoDriver();
    pVideoDrv->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
    
    m_pColWorld->GetColWorld()->debugDrawWorld();
    
    set<CElement*>::const_iterator it;
    const set<CElement*> &Elements = m_pElementsMgr->GetByType(OFET_ENTITY);
    for(it = Elements.begin(); it != Elements.end(); ++it)
    {
        CEntity *pEntity = static_cast<CEntity*>(*it);
        if(pEntity->GetUid() == OFE_INVALID_UID)
            continue;
        
        irr::core::vector3df vCamPos = m_pGame->GetCamera()->getPosition();
        btVector3 vDist = btVector3(vCamPos.X, vCamPos.Y, vCamPos.Z) - pEntity->GetPos();
        if(vDist.length2() < 100.0f)
            pEntity->DbgDraw();
    }
        
#endif // DEBUG
}

void CLevel::AddElement(CElement *pElement)
{
    m_pElementsMgr->Add(pElement);
    
    /*if(CObject::IsObject(pElement))
    {
        m_ColWorld.addRigidBody(((CObject*)pElement)->GetColObject(), 1 << pElement->GetType(), btBroadphaseProxy::AllFilter);
        m_ColWorld.addCollisionObject(((CObject*)pElement)->GetColObject(), 1 << pElement->GetType(), btBroadphaseProxy::AllFilter);
    }*/
}

void CLevel::RemoveElement(CElement *pElement)
{
    m_pElementsMgr->Remove(pElement);
    
    //if(CObject::IsObject(pElement) && ((CObject*)pElement)->GetColObject()->isInWorld())
    //    m_ColWorld.removeRigidBody(((CObject*)pElement)->GetColObject());
    
    if(pElement->GetType() == OFET_ENTITY)
    {
        const set<CElement*> &Projectiles = m_pElementsMgr->GetByType(OFET_PROJECTILE);
        for(set<CElement*>::iterator it = Projectiles.begin(); it != Projectiles.end(); ++it)
        {
            CProjectile * const pProj = (CProjectile*)*it;
            if(pProj->GetEntity() == pElement)
                pProj->SetEntity(NULL);
        }
    }
}

int CLevel::ReadRflString(std::string &strDest, CInputBinaryStream &Stream)
{
    unsigned nLen = Stream.ReadUInt16();
    if(!Stream.good())
        return OFE_FILE_READ_ERROR;
    
    char *pStr = new char[nLen + 1]; // FIXME: how to make it better?
    Stream.ReadBinary(pStr, nLen);
    if(!Stream.good())
        return OFE_FILE_READ_ERROR;
    
    pStr[nLen] = 0;
    strDest = pStr;
    delete pStr;
    
    return 0;
}

int CLevel::IgnoreRflString(CInputBinaryStream &Stream)
{
    unsigned nLen = Stream.ReadUInt16();
    
    Stream.ignore(nLen);
    return 0;
}

