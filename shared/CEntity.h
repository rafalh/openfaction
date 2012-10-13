/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CEntity.h
*  PURPOSE:     Entity representation e.g. humans, animals and vehicles
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CENTITY_H
#define CENTITY_H

#include "rfproto.h"
#include "CKillableObject.h"
#include "CInputBinaryStream.h"
#include "CWeaponsTable.h"
#include "CEntityState.h"
#include "CEntityAction.h"
#include <map>

class CPlayer;
class CAnimatedMesh;
class CWeapon;
class CAnimation;
struct SEntityClass;
struct SAmmoType;
class btKinematicCharacterController;

/*class CEntityMotionState: public btMotionState
{
    public:
        CEntityMotionState(const btTransform &Transform = btTransform::getIdentity()):
            m_Transform(Transform) {}
        
        void getWorldTransform(btTransform &Transform) const
        {
            Transform = m_Transform;
        }
        
        void setWorldTransform(const btTransform &Transform)
        {
            return;
        }
    
    private:
        btTransform m_Transform;
};*/

class CEntity: public CKillableObject
{
    public:
        CEntity(CLevel *pLevel, const SEntityClass *pClass, unsigned nUid = OFE_INVALID_UID);
        CEntity(CLevel *pLevel, CInputBinaryStream &Stream);
        ~CEntity();
        void Damage(float fDmg, SDamageInfo &DmgInfo = SDamageInfo::Default);
        void Kill(SDamageInfo &DmgInfo = SDamageInfo::Default);
        void SetLife(float nLife);
        void SetArmor(float fArmor);
        
        const SEntityClass *GetClass() const
        {
            return m_pClass;
        }
        
        unsigned GetHandle() const
        {
            return m_Handle;
        }
        
        float GetArmor() const
        {
            return m_fArmor;
        }
        
        bool IsLifeArmorChanged() const
        {
            return m_bLifeArmorChanged;
        }
        
        void SetLifeArmorChanged(bool bChanged)
        {
            m_bLifeArmorChanged = bChanged;
        }
        
        void Move(const btVector3 &Pos)
        {
            if(GetPos() != Pos)
            {
                CObject::SetPos(Pos);
                m_bPosRotAnimChanged = true;
            }
        }
        
        void SetPos(const btVector3 &Pos)
        {
            Move(Pos);
            m_OldTransform = m_pColObj->getWorldTransform();
        }
        
        const btMatrix3x3 &GetRotMatrix() const
        {
            return m_CamRotMatrix;
        }
        
        void SetRotMatrix(const btMatrix3x3 &matRot)
        {
            if(!(m_CamRotMatrix == matRot)) // there is no != operator
            {
                m_CamRotMatrix = matRot;
                m_bPosRotAnimChanged = true;
            }
        }
        
        unsigned GetAnimation() const
        {
            return m_nAnimation;
        }
        
        void SetAnimation(unsigned nAnimation)
        {
            if(m_nAnimation != nAnimation)
            {
                m_nAnimation = nAnimation;
                m_bPosRotAnimChanged = true;
            }
        }
        
        bool IsPosRotAnimChanged() const
        {
            return m_bPosRotAnimChanged;
        }
        
        void SetPosRotAnimChanged(bool bChanged)
        {
            m_bPosRotAnimChanged = bChanged;
        }
        
        const CEntityState &GetState() const
        {
            return m_State;
        }
        
        const CEntityAction &GetAction() const
        {
            return m_Action;
        }
        
        const CEntityState &SetState(const CEntityState &State)
        {
            m_State = State;
        }
        
        const CEntityAction &SetAction(const CEntityAction &Action)
        {
            m_Action = Action;
        }
        
        btVector3 GetHeadPos() const
        {
            static const btVector3 vHeadOffset(0.0f, 0.8724f, 0.0f);
            return GetPos() + vHeadOffset;
        }
        
        CWeapon *GetWeapon(const SWeaponClass *pClass)
        {
            std::map<unsigned, CWeapon*>::iterator it = m_Weapons.find(pClass->nId);
            if(it == m_Weapons.end())
                return NULL;
            
            return it->second;
        }
        
        CWeapon *GetWeapon()
        {
            assert(m_pWeapon);
            return m_pWeapon;
        }
        
        bool SwitchWeapon(const SWeaponClass *pClass);
        void RemoveWeapons();
        unsigned GetAmmo(const SAmmoType *pAmmoType = NULL);
        
        void SetAmmo(const SAmmoType *pAmmoType, unsigned nAmmo)
        {
            m_Ammo[pAmmoType->nId] = nAmmo;
        }
        
        bool AddAmmo(const SWeaponClass *pWepClass, unsigned nAmmo);
        CWeapon *AddWeapon(const SWeaponClass *pClass, unsigned nAmmo = 0);
        bool ReloadWeapon(const SWeaponClass *pClass = NULL, bool bWait = true);
		bool Shoot(bool bAltFire = false, bool bWait = true);
		bool Shoot(const btVector3 &vPos, const btVector3 &vDir, bool bAltFire = false, bool bWait = true);
#if defined(DEBUG) && defined(OF_CLIENT)
		void DbgDraw() const;
#endif // defined(DEBUG) && defined(OF_CLIENT)
        
        bool IsWeaponChanged() const
        {
            return m_bWeaponChanged;
        }
        
        void SetWeaponChanged(bool bChanged)
        {
            m_bWeaponChanged = bChanged;
        }
        
        bool IsInvulnerabilityEnabled() const
        {
            return !!m_nInvulnerability;
        }
        
        void SetInvulnerability(unsigned nSeconds = 15)
        {
            if(!!m_nInvulnerability != !!nSeconds)
            {
                m_nInvulnerability = nSeconds * 1000;
                m_bArmorStateChanged = true;
            }
        }
        
        bool IsDamageAmplifierEnabled() const
        {
            return !!m_nDamageAmplifier;
        }
        
        void SetDamageAmplifier(unsigned nSeconds = 30)
        {
            if(!!m_nDamageAmplifier != !!nSeconds)
            {
                m_nDamageAmplifier = nSeconds * 1000;
                m_bArmorStateChanged = true;
            }
        }
        
        bool IsArmorStateChanged() const
        {
            return m_bArmorStateChanged;
        }
        
        void SetArmorStateChanged(bool bChanged)
        {
            m_bArmorStateChanged = bChanged;
        }
        
        void Update(unsigned nDeltaTime);
        
        CPlayer *GetPlayer() const
        {
            return m_pPlayer;
        }
        
        void SetPlayer(CPlayer *pPlayer)
        {
            m_pPlayer = pPlayer;
        }
        
        btKinematicCharacterController *GetCharacter() const
        {
            return m_pCharacter;
        }
    
    private:
        const SEntityClass *m_pClass;
        unsigned m_Handle;
        
        float m_fArmor;
        bool m_bLifeArmorChanged;
        
        btTransform m_OldTransform;
        btMatrix3x3 m_CamRotMatrix;
        unsigned m_nAnimation; // FIXME: Use state and action instead
        bool m_bPosRotAnimChanged;
        
        CEntityState m_State;
        CEntityAction m_Action;
        
        std::map<unsigned, CWeapon*> m_Weapons;
        CWeapon *m_pWeapon;
        unsigned *m_Ammo;
        bool m_bWeaponChanged;
        
        unsigned m_nInvulnerability;
        unsigned m_nDamageAmplifier;
        bool m_bArmorStateChanged;
        
        CPlayer *m_pPlayer;
        
        btConvexShape *m_pColShape;
        //CEntityMotionState m_MotionState;
        btDefaultMotionState m_MotionState;
        btKinematicCharacterController *m_pCharacter;
        
        CAnimatedMesh *m_pMesh;
        
        std::map<int, std::map<CEntityState, CAnimation*> > m_States;
        std::map<int, std::map<CEntityAction, CAnimation*> > m_Actions;
        
        void LoadAnimations();
        void FreeAnimations();
};

#endif // CENTITY_H
