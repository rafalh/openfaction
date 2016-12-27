/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CMaterial.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CMultiTexture.h"
#include "vbm_format.h"
#include "CTextureMgr.h"
#include "CException.h"
#include "CConsole.h"
#include "CLogger.h"
#include "irr/CReadFile.h"
#include "irr/CVbmAnimator.h"

unsigned CMultiTexture::m_UniqueId = 0;

CMultiTexture::CMultiTexture(CTextureMgr *pTextureMgr, const CString &strFilename):
    m_pTextureMgr(pTextureMgr),
    m_Fps(0), m_bAnimated(false), m_bAlpha(false),
    m_bInvisible(false)
{
    if(strFilename == "sld_invisible01.tga" || strFilename == "mtl_invisible02.tga" || strFilename == "cem_invisible03.tga" || strFilename == "rck_invisible04.tga")
        m_bInvisible = true;
    
    try
    {
        // Create Irrlicht reader
        CIrrReadFile IrrReadFile(strFilename);
        
        // Load standard image
        irr::video::IImage *pImg = m_pTextureMgr->GetVideoDriver()->createImageFromFile(&IrrReadFile);
        
        if(pImg)
        {
            // Check if image has alhpa channel.
            // Note: video::ITexture::hasAlpha() usually returns true because texture is loaded in 32bpp mode even if file doesn't have alpha
            if(pImg->getAlphaMask())
                m_bAlpha = true;
            
            // Create texture
            irr::video::ITexture *pTexture = m_pTextureMgr->GetVideoDriver()->addTexture(strFilename.c_str(), pImg);
            pImg->drop();
            assert(pTexture);
            m_Frames.push_back(pTexture);
        }
        else
        {
            // Load a VBM animation
            CVfsFileStream File(strFilename);
            LoadVbm(File);
            m_bAnimated = true;
        }
    }
    catch(const CException &e)
    {
        CLogger::GetInst().PrintError("Failed to load texture %s (%s)\n", strFilename.c_str(), e.what());
        // TODO
    }
}

CMultiTexture::~CMultiTexture()
{
    // Destroy frames
    for(unsigned i = 0; i < m_Frames.size(); ++i)
        m_pTextureMgr->GetVideoDriver()->removeTexture(m_Frames[i]);
    m_Frames.clear();
    
    // Remove ourself from materials manager
    m_pTextureMgr->Remove(this);
}

bool CMultiTexture::LoadVbm(CInputBinaryStream &Stream)
{
    vbm_header_t Hdr;
    Stream.ReadBinary(&Hdr, sizeof(Hdr));
    
    if(Hdr.signature != VBM_SIGNATURE)
        return false;
    
    assert(Hdr.version <= 2);
    
    assert(Hdr.fps < 100);
    m_Fps = Hdr.fps;
    
    assert(Hdr.width <= 2048);
    assert(Hdr.height <= 2048);
    assert(Hdr.format <= 2);
    
    if(Hdr.format == VBM_CF_1555 || Hdr.format == VBM_CF_4444)
        m_bAlpha = true;
    
    for(unsigned i = 0; i < Hdr.frames_count; ++i)
    {
        vbm_pixel_t *pBuf = new vbm_pixel_t[Hdr.width * Hdr.height];
        Stream.ReadBinary(pBuf, Hdr.width * Hdr.height * sizeof(vbm_pixel_t));
        
        irr::video::IImage *pImg = m_pTextureMgr->GetVideoDriver()->createImage(
            irr::video::ECF_A8R8G8B8, irr::core::dimension2d<irr::u32>(Hdr.width, Hdr.height)); // ECF_R5G6B5 doesnt work
        for (unsigned y = 0; y < Hdr.height; ++y)
            for(unsigned x = 0; x < Hdr.width; ++x)
            {
                unsigned r, g, b, a;
                vbm_pixel_t &Src = pBuf[y * Hdr.width + x];
                
                switch(Hdr.format)
                {
                    case VBM_CF_1555:
                        a = Src.cf_1555.nalpha ? 0 : 255;
                        r = Src.cf_1555.red << 3;
                        g = Src.cf_1555.green << 3;
                        b = Src.cf_1555.blue << 3;
                        break;
                    case VBM_CF_4444:
                        a = Src.cf_4444.alpha << 4;
                        r = Src.cf_4444.red << 4;
                        g = Src.cf_4444.green << 4;
                        b = Src.cf_4444.blue << 4;
                        break;
                    case VBM_CF_565:
                        a = 255;
                        r = Src.cf_565.red << 3;
                        g = Src.cf_565.green << 2;
                        b = Src.cf_565.blue << 3;
                        break;
                    default:
                        assert(false);
                }
                
                pImg->setPixel(x, y, irr::video::SColor(a, r, g, b));
            }
        
        char szName[32];
        sprintf(szName, "__%u", m_UniqueId++);
        irr::video::ITexture *pTexture = m_pTextureMgr->GetVideoDriver()->addTexture(szName, pImg);
        
        m_Frames.push_back(pTexture);
        
        pImg->drop();
        delete[] pBuf;
        
        //break; // FIXME
        //for(unsigned j = 1; j < Hdr.mipmaps_count; ++j)
        //    Stream.ignore()
    }
    
    return true;
}

irr::scene::ISceneNodeAnimator *CMultiTexture::CreateAnimator(unsigned iNodeMaterial)
{
    return new CIrrVbmAnimator(this, iNodeMaterial);
}
