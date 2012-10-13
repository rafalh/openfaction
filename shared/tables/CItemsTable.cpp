/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/tables/CItemsTable.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "utils.h"
#include "CItemsTable.h"
#include "CTblReader.h"
#include "CLogger.h"
#include "CGame.h"
#include "CException.h"

using namespace std;

static const char *g_ItemFlags[] = { "no_pickup", "spins_in_multi" };

SItemClass::SItemClass(unsigned _nId):
    nId(_nId)
{
    Type = IT_UNKNOWN;
    pWeaponClass = NULL;
    bWeapon = false;
    nCount = 0;
    Flags = 0;
}

int CItemsTable::Load(istream &Stream)
{
    CTblReader Reader(Stream, "Items");
    if(!Reader.IsSectionFound())
        return -1;
    
    while(Reader.LoadNextElement() == 0)
    {
        const char *pName = Reader.GetName();
        
        if(!StrCmpI(pName, "$Class Name"))
        {
            SItemClass Item(m_Items.size());
            Reader.GetString(Item.strName);
            Item.Type = GetTypeFromClsName(Item.strName);
            
            m_Items.push_back(Item);
        }
        else if(m_Items.size())
        {
            if(!StrCmpI(pName, "$V3D Filename"))
            {
                Reader.GetString(m_Items.back().strMeshFilename);
            }
            else if(!StrCmpI(pName, "$V3D Type"))
            {
                CString strType;
                Reader.GetString(strType);
                
                size_t Pos = m_Items.back().strMeshFilename.rfind(".");
                if(strType == "static" && Pos != CString::npos)
                    m_Items.back().strMeshFilename.replace(Pos, 4, ".v3m");
                else if(strType == "anim" && Pos != CString::npos)
                    m_Items.back().strMeshFilename.replace(Pos, 4, ".vfx");
                else
                    THROW_EXCEPTION("Invalid V3D Type: %s", strType.c_str());
            }
            else if(!StrCmpI(pName, "$Gives Weapon"))
            {
                CString strWeapon;
                Reader.GetString(strWeapon);
                const SWeaponClass *pWeaponClasss = m_pGame->GetWeaponsTbl()->Get(strWeapon);
                if(pWeaponClasss)
                {
                    m_Items.back().pWeaponClass = pWeaponClasss;
                    m_Items.back().bWeapon = true;
				}
            }
            else if(!StrCmpI(pName, "$Ammo For"))
            {
                CString strWeapon;
                Reader.GetString(strWeapon);
                m_Items.back().pWeaponClass = m_pGame->GetWeaponsTbl()->Get(strWeapon);
            }
            else if(!StrCmpI(pName, "$Count"))
            {
                Reader.GetUInt(m_Items.back().nCount);
            }
            else if(!StrCmpI(pName, "$Count Multi"))
            {
                Reader.GetUInt(m_Items.back().nCount);
            }
            else if(!StrCmpI(pName, "$Flags"))
            {
                Reader.GetFlags(m_Items.back().Flags, g_ItemFlags, COUNTOF(g_ItemFlags));
            }
        }
    }
    
    return 0;
}

const SItemClass *CItemsTable::Get(const CString &strClassName)
{
    for(unsigned i = 0; i < m_Items.size(); ++i)
        if(!m_Items[i].strName.comparei(strClassName))
            return &m_Items[i];
    
    CLogger::GetInst().PrintError("Warning! Unknown weapon class name: %s", strClassName.c_str());
    return NULL;
}

const SItemClass *CItemsTable::FindWeapon(const SWeaponClass *pClass)
{
    for(unsigned i = 0; i < m_Items.size(); ++i)
        if(m_Items[i].pWeaponClass == pClass)
            return &m_Items[i];
    
    return NULL;
}

EItemType CItemsTable::GetTypeFromClsName(const CString &strItemClsName)
{
    if(!strItemClsName.comparei("Medical Kit") ||
       !strItemClsName.comparei("First Aid Kit"))
        return IT_MEDICAL_KIT;
    else if(!strItemClsName.comparei("Suit Repair"))
        return IT_SUIT_REPAIR;
    else if(!strItemClsName.comparei("Miner Envirosuit"))
        return IT_MINER_ENVIROSUIT;
    else if(!strItemClsName.comparei("Multi Invulnerability"))
        return IT_MULTI_INVULNERABILITY;
    else if(!strItemClsName.comparei("Multi Damage Amplifier"))
        return IT_MULTI_DMG_AMP;
    else if(!strItemClsName.comparei("Multi Super Armor"))
        return IT_MULTI_SUPER_ARMOR;
    else if(!strItemClsName.comparei("Multi Super Health"))
        return IT_MULTI_SUPER_HEALTH;
    else
        return IT_UNKNOWN;
}
