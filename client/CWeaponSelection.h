/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CWeaponSelection.h
*  PURPOSE:     Weapon selection GUI
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CWEAPONSELECTION_H
#define CWEAPONSELECTION_H

#include <irrlicht.h>
#include "CWeaponsTable.h"
#include "CTimer.h"

class CGame;
class CEntity;
class CMaterial;
class CWeapon;

class CWeaponSelection: public irr::IEventReceiver
{
    public:
        CWeaponSelection(CGame *pGame);
        ~CWeaponSelection();
        void Render();
        bool OnEvent(const irr::SEvent &Event);
        
        void SetEntity(CEntity *pEntity)
        {
            m_pEntity = pEntity;
        }
    
    private:
        CGame *m_pGame;
        CEntity *m_pEntity;
        CMaterial *m_Textures[256];
        
        EWeaponType m_WeaponType;
        unsigned m_iWeaponCls;
        
        bool m_bVisible;
        CTimer m_Timer;
        
        static const unsigned VISIBLE_TIME;
        
        CWeapon *FindWeaponFromType(EWeaponType Type, unsigned iHint = 0u);
};

#endif // CWEAPONSELECTION_H
