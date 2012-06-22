/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CLevel.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CLEVEL_H
#define CLEVEL_H

#include <vector>
#include "CInputBinaryStream.h"
#ifdef OF_SERVER
# include "CChecksum.h"
#endif

class CGame;
class CElement;
class CItem;
class CColWorld;
class CElementsMgr;
class CLightmaps;
class CStaticGeometry;
class CLevelProperties;
class CSpawnpointsManager;
struct SItemClass;

class CLevel
{
    public:
        CLevel(CGame *pGame);
        ~CLevel();
        void Load(CInputBinaryStream &Stream);
        void Unload();
        void Update(unsigned nDeltaTime);
        CItem *GetItemFromClass(const SItemClass *pClass);
        void DbgDraw() const;
        
        CGame *GetGame() const
        {
            return m_pGame;
        }
        
        const char *GetName() const
        {
            return m_strName.c_str();
        }
        
        CColWorld *GetColWorld()
        {
            return m_pColWorld;
        }
        
        CElementsMgr *GetElementsMgr()
        {
            return m_pElementsMgr;
        }
        
        const CLevelProperties *GetProperties() const
        {
            return m_pLvlProp;
        }
        
        const btVector3 &GetStartPos() const
        {
            return m_vPlayerStartPos;
        }
#ifdef OF_SERVER
        uint32_t GetChecksum() const
        {
            return m_Checksum.GetValue();
        }
        
        const CSpawnpointsManager *GetSpawnpointsMgr() const
        {
            return m_pSpawnpointsMgr;
        }
#else
        CLightmaps *GetLightmaps()
        {
            return m_pLightmaps;
        }
#endif
        void QueueDelete(CElement *pElement)
        {
            m_DeleteQueue.push_back(pElement);
        }
        
        void AddElement(CElement *pElement);
        void RemoveElement(CElement *pElement);
        
    private:
        int ReadRflString(std::string &strDest, CInputBinaryStream &Stream);
        int IgnoreRflString(CInputBinaryStream &Stream);
        
        CGame *m_pGame;
        std::string m_strName;
        CColWorld *m_pColWorld;
        CElementsMgr *m_pElementsMgr;
        std::vector<CElement*> m_DeleteQueue;
        CLevelProperties *m_pLvlProp;
#ifdef OF_SERVER
        CChecksum m_Checksum;
        CSpawnpointsManager *m_pSpawnpointsMgr;
#else // !OF_SERVER
        CLightmaps *m_pLightmaps;
#endif // OF_SERVER
        btVector3 m_vPlayerStartPos;
        btMatrix3x3 m_PlayerStartRotMatrix;
        
        CStaticGeometry *m_pStaticGeometry;
};

#endif // CLEVEL_H
