/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CEntityState.h
*  PURPOSE:     Entity states list
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef EENTITYSTATE_H_INCLUDED
#define EENTITYSTATE_H_INCLUDED

#include <cassert>
#include "utils.h"

class CEntityState
{
    public:
        enum EState
        {
            ES_STAND,
            ES_ATTACK_STAND,
            ES_WALK,
            ES_ATTACK_WALK,
            ES_RUN,
            ES_ATTACK_RUN,
            ES_FLEE_RUN,
            ES_FLAIL_RUN,
            ES_CROUCH,
            ES_ATTACK_CROUCH,
            ES_ATTACK_CROUCH_WALK,
            ES_ATTACK_LEAN_LEFT,
            ES_ATTACK_LEAN_RIGHT,
            ES_COWER,
            ES_FREEFALL,
            ES_ON_TURRET,
            ES_CORPSE_CARRY_STAND,
            ES_CORPSE_CARRY_WALK,
            ES_SWIM_STAND,
            ES_SWIM_WALK,
            ES_JEEP_DRIVE,
            ES_JEEP_GUN,
            ES_CUSTOM,
            
            ES_COUNT,
        };
        
        CEntityState(EState State):
            m_State(State) {}
        
        CEntityState(const char *pszState)
        {
            m_State = GetStateFromName(pszState);
        }
        
        const char *GetName() const
        {
            //assert(ES_COUNT == COUNTOF(StateNames));
            return GetName(m_State);
        }
    
        bool operator==(const CEntityState &State) const
        {
            return m_State == State.m_State;
        }
        
        bool operator<(const CEntityState &State) const
        {
            return m_State < State.m_State;
        }
        
        bool operator>(const CEntityState &State) const
        {
            return m_State > State.m_State;
        }
    
    private:
        EState m_State;
        
        static const char *GetName(EState State)
        {
            static const char *StateNames[] = {
                "stand",
                "attack_stand",
                "walk",
                "attack_walk",
                "run",
                "attack_run",
                "flee_run",
                "flail_run",
                "crouch",
                "attack_crouch",
                "attack_crouch_walk",
                "attack_lean_left",
                "attack_lean_right",
                "cower",
                "freefall",
                "on_turret",
                "corpse_carry_stand",
                "corpse_carry_walk",
                "swim_stand",
                "swim_walk",
                "jeep_drive",
                "jeep_gun",
                "custom",
            };
            
            assert((int)State < COUNTOF(StateNames));
            return StateNames[(int)State];
        }
        
        static EState GetStateFromName(const char *pszState)
        {
            for(unsigned i = 0; i < ES_COUNT; ++i)
                if(!strcmp(GetName((EState)i), pszState))
                    return (EState)i;
            return ES_STAND;
        }
};

#endif // EENTITYSTATE_H_INCLUDED
