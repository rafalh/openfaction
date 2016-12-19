/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        packets/CEntityUpdatePacket.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CEntityUpdatePacket.h"
#include "CServer.h"
#include "CLogger.h"
#include "CWeapon.h"
#include "CGame.h"

using namespace std;

void CEntityUpdatePacket::Send(CPlayer *pPlayer, bool bSyncAll) const
{
    COutputBinaryStringStream Stream;
    
    for(vector<CEntity*>::const_iterator it = m_Entities.begin(); it < m_Entities.end(); ++it)
    {
        CEntity * const pEntity = *it;
        CWeapon * const pWeapon = pEntity->GetWeapon();
        
        unsigned nFlags = 0;
        
        if(pEntity->IsLifeArmorChanged() || bSyncAll)
            nFlags |= RF_EDF_HEATH_ARMOR;
        
        if(pEntity->IsArmorStateChanged() || bSyncAll)
            nFlags |= RF_EDF_ARMOR_STATE;
        
        if(pEntity->IsWeaponChanged() || bSyncAll)
            nFlags |= RF_EDF_WEAPON_TYPE;
        
        if(pEntity != pPlayer->GetEntity())
        {
            if(pEntity->IsPosRotAnimChanged() || bSyncAll ||
               pWeapon->GetState() != CWeapon::WS_IDLE) // HACKFIX: animation was broken in continuous fire
            {
                nFlags |= RF_EDF_POS_ROT_ANIM;
            }
            
            if(pWeapon->GetState() == CWeapon::WS_FIRE)
                nFlags |= RF_EDF_FIRE;
            
            if(pWeapon->GetState() == CWeapon::WS_ALT_FIRE)
                nFlags |= RF_EDF_FIRE|RF_EDF_ALT_FIRE;
        }
        
        if(nFlags)
        {
            COutputBinaryStringStream Stream2;
            
            Stream2.WriteUInt32(pEntity->GetHandle());
            
            Stream2.WriteUInt8(nFlags);
            
            if(nFlags & RF_EDF_POS_ROT_ANIM)
            {
                Stream2.WriteUInt16(g_AppTimer.GetValue());
                Stream2.WriteVector(pEntity->GetPos());
                
                // Algohritm based on rotation loading part
                btVector3 vFw = pEntity->GetRotMatrix().getColumn(2);
                float fYaw = atan2f(vFw[0], vFw[2]); // Factor doesn't change yaw
                
                float fPitchSgn = vFw[1] >= 0 ? 1.0f : -1.0f;
                float fFactor = vFw[0] / (vFw[0] + fPitchSgn * vFw[1] * sinf(fYaw));
                assert(fFactor >= 0.0f);
                vFw[0] /= fFactor;
                vFw[2] /= fFactor;
                vFw.normalize();
                
                float fPitch = asinf(vFw[1]);
                int iPitch = (int)(fPitch * 32168.0f / M_PI_2);
                int iYaw = (int)(fYaw * 32510.0f / M_PI / 2.0f);
                Stream2.WriteInt16(iPitch);
                Stream2.WriteInt16(iYaw);
                
                uint32_t uAnim = pEntity->GetAnimation();
                Stream2.WriteUInt32(uAnim);
            }
            
            if(nFlags & RF_EDF_ARMOR_STATE)
            {
                unsigned nState = 0;
                if(pEntity->IsDamageAmplifierEnabled())
                    nState |= RF_EDAS_DAMAGE_AMPLIFIER;
                if(pEntity->IsInvulnerabilityEnabled())
                    nState |= RF_EDAS_INVULNERABILITY;
                Stream2.WriteUInt8(nState);
            }
            
            if(nFlags & RF_EDF_WEAPON_TYPE)
                Stream2.WriteUInt8(pWeapon->GetClass()->nId);
            
            if(nFlags & RF_EDF_HEATH_ARMOR)
            {
                Stream2.WriteUInt8((uint8_t)pEntity->GetLife());
                Stream2.WriteUInt8((uint8_t)pEntity->GetArmor());
                Stream2.WriteUInt8(0);
            }
            
            if(nFlags & RF_EDF_UNKNOWN4)
                Stream2.WriteUInt16(0);
            
            if(4 + Stream.str().size() + Stream2.str().size() > MAX_PACKET_SIZE)
            {
                Stream.WriteUInt32(0xFFFFFFFF);
                pPlayer->GetConnection()->AddPacket(RF_ENTITY_UPDATE, Stream.str(), false);
                Stream.str("");
            }
            
            Stream.WriteBinary(Stream2.str().data(), Stream2.str().size());
        }
    }
    
    Stream.WriteUInt32(0xFFFFFFFF);
    pPlayer->GetConnection()->AddPacket(RF_ENTITY_UPDATE, Stream.str(), false);
}

void CEntityUpdatePacket::Process(CInputBinaryStream &Stream, CPlayer *pPlayer)
{
    while(Stream.good())
    {
        unsigned hEntity = Stream.ReadUInt32();
        if(hEntity == 0xFFFFFFFF || !Stream.good())
            break;
        
        CEntity * const pEntity = pPlayer->GetEntity();
        if(!pEntity || hEntity != pEntity->GetHandle())
        {
            CLogger::GetInst().PrintStatus("Invalid entity handle: %x != %x", hEntity, pEntity ? pEntity->GetHandle() : 0);
            break;
        }
        
        unsigned nFlags = Stream.ReadUInt8();
        unsigned nUnknownFlags = nFlags & (~(RF_EDF_POS_ROT_ANIM|RF_EDF_FIRE|RF_EDF_ALT_FIRE|RF_EDF_UNKNOWN3|RF_EDF_UNKNOWN4));
        if(nUnknownFlags)
            CLogger::GetInst().PrintStatus("Unknown object update flags: %02x", nUnknownFlags);
        
        CWeapon *pWeapon = pEntity->GetWeapon();
        if(pWeapon)
        {
            CWeapon::EState WeaponState;
            if(nFlags & RF_EDF_FIRE)
                WeaponState = CWeapon::WS_FIRE;
            else if(nFlags & RF_EDF_ALT_FIRE)
                WeaponState = CWeapon::WS_ALT_FIRE;
            else
                WeaponState = CWeapon::WS_IDLE;
            pWeapon->SetState(WeaponState);
        }
        
        if(nFlags & RF_EDF_POS_ROT_ANIM)
        {
            Stream.ignore(2); // ticks
            btVector3 Pos = Stream.ReadVector();
            
            // Update position
            pEntity->Move(Pos);
            
            // Update rotation matrix
            int iPitch = Stream.ReadInt16();
            assert(-32168 <= iPitch && iPitch <= 32168);
            float fPitch = iPitch / 32168.001f * (float)M_PI_2;
            
            int iYaw = Stream.ReadInt16();
            assert(-32510 <= iYaw && iYaw <= 32510);
            float fYaw = iYaw / 32510.0f * 2.0f * (float)M_PI;
            
            // This is how RF created forward vector
            btVector3 vFw;
            vFw[1] = sinf(fPitch);
            float fFactor = 1.0f - fabs(vFw[1]);
            vFw[0] = fFactor * sinf(fYaw);
            vFw[2] = fFactor * cosf(fYaw);
            vFw.normalize();
            
            // Use up vector to generate right vector
            static const btVector3 vUp(0.0f, 1.0f, 0.0f);
            btMatrix3x3 matRot;
            matRot[0] = vUp.cross(vFw);
            matRot[1] = vUp;
            matRot[2] = vFw;
            matRot = matRot.transpose();
            pEntity->SetRotMatrix(matRot);
            
            // Update animation - needs more work
            uint32_t uAnim = Stream.ReadUInt32();
            pEntity->SetAnimation(uAnim);
        }
        
        /* HACK: Bug in rf? It doesnt set weapon flag. Needs more work... */
        //if(nFlags & RF_ODF_WEAPON_TYPE)
        //{
            unsigned nWeaponType = Stream.ReadUInt8();
            const SWeaponClass *pWepClass = pPlayer->GetServer()->GetGame()->GetTables()->weapons()->Get(nWeaponType);
            if(pWepClass && pWepClass != pEntity->GetWeapon()->GetClass())
            {
                if(!pEntity->SwitchWeapon(pWepClass))
                    CLogger::GetInst().PrintStatus("%s failed to switch weapon to %02x!", pPlayer->GetName(), nWeaponType);
            }
        //}
        
        if(nFlags & RF_EDF_UNKNOWN3)
        {
            unsigned unknown3 = Stream.ReadUInt8();
            Stream.ignore(unknown3 * 3);
        }
                
        if(nFlags & RF_EDF_UNKNOWN4)
            Stream.ignore(2);
    }
}
