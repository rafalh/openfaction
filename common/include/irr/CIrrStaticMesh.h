#pragma once

#include <irrlicht.h>
#include <CV3dMeshData.h>

class CTextureMgr;
class CMultiTexture;

class CIrrStaticMesh : public irr::scene::SMesh
{
public:
    CIrrStaticMesh(irr::video::IVideoDriver *videoDriver, CTextureMgr *textureMgr) :
        m_videoDriver(videoDriver), m_textureMgr(textureMgr) {}
    ~CIrrStaticMesh();
    void init(v3d::CV3dMeshData *meshData, int subMeshIndex = 0, int lodIndex = 0);
    void renderBones(const irr::core::vector3df &pos, irr::IrrlichtDevice *device);

private:
    irr::video::IVideoDriver *m_videoDriver;
    CTextureMgr *m_textureMgr;
    std::vector<CMultiTexture*> m_textures;
    std::vector<v3d::CV3dMeshData::SBone> m_bones;
};
