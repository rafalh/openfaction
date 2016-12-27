/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CStaticGeometry.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSTATICGEOMETRY_H
#define CSTATICGEOMETRY_H

#include "CKillableObject.h"
#include <btBulletCollisionCommon.h>
#include <vector>
#include <algorithm>
#include <util/CString.h>

class CInputBinaryStream;
class CMultiTexture;

class CRoom: public CKillableObject
{
    public:
        CRoom(CLevel *pLevel, unsigned nId, float fLife = -1.0f, bool bDetail = false);
        ~CRoom();
        void Kill(SDamageInfo &DmgInfo);
        
        inline bool IsDetail() const
        {
            return m_bDetail;
        }
        
        inline bool IsGlass() const
        {
            return m_bDetail && m_fLife >= 0.0f;
        }
        
        inline unsigned GetGlassIndex() const
        {
            return m_GlassIndex;
        }
    
    private:
        btTriangleMesh *m_pMesh;
        btBvhTriangleMeshShape *m_pShape;
        btDefaultMotionState m_MotionState;
        btVector3 m_vCenter;
        
        unsigned m_GlassIndex;
        bool m_bDetail;
        bool m_bSkyRoom;
        
    friend class CStaticGeometry;
};

class CStaticGeometry
{
    public:
        inline ~CStaticGeometry()
        {
            Unload();
        }
        
        inline const std::vector<CRoom*> &GetRooms() const
        {
            return m_Rooms;
        }
        
        void Load(CLevel *pLevel, CInputBinaryStream &Stream, unsigned nVersion);
        void Unload();
    
    private:
        std::vector<CRoom*> m_Rooms;
        std::vector<CMultiTexture*> m_Materials;

        bool IsInvisibleLevelTexture(const CString &strFilename)
        {
            return strFilename == "sld_invisible01.tga" || strFilename == "mtl_invisible02.tga" ||
                strFilename == "cem_invisible03.tga" || strFilename == "rck_invisible04.tga";
        }
};

#endif // CSTATICGEOMETRY_H
