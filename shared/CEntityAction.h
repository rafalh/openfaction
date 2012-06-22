/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CEntityAction.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CENTITYACTION_H
#define CENTITYACTION_H

#include <cassert>

class CEntityAction
{
    public:
        enum EAction
        {
            EA_CORPSE_DROP,
            EA_CORPSE_CARRY,
            EA_FIRE_STAND,
            EA_ALT_FIRE_STAND,
            EA_FIRE_CROUCH,
            EA_DEATH_GENERIC,
            EA_DEATH_BLAST_FORWARD,
            EA_DEATH_BLAST_BACKWARD,
            EA_DEATH_HEAD_FORWARD,
            EA_DEATH_HEAD_BACKWARD,
            EA_DEATH_HEAD_NEUTRAL,
            EA_DEATH_CHEST_FORWARD,
            EA_DEATH_CHEST_BACKWARD,
            EA_DEATH_CHEST_NEUTRAL,
            EA_DEATH_LEG_LEFT,
            EA_DEATH_LEG_RIGHT,
            EA_DEATH_CROUCH,
            EA_SIDESTEP_LEFT,
            EA_SIDESTEP_RIGHT,
            EA_ROLL_LEFT,
            EA_ROLL_RIGHT,
            EA_LAND,
            EA_FLINCH_STAND,
            EA_FLINCH_ATTACK_STAND,
            EA_FLINCH_CHEST,
            EA_FLINCH_BACK,
            EA_FLINCH_LEG_LEFT,
            EA_FLINCH_LEG_RIGHT,
            EA_IDLE_TO_READY,
            EA_READY_TO_IDLE,
            EA_IDLE_1,
            EA_IDLE_2,
            EA_IDLE_3,
            EA_IDLE_4,
            EA_ROCK_DROP,
            EA_ROCK_PICKUP,
            EA_DEATH_STILL_1,
            EA_DEATH_STILL_2,
            EA_DEATH_STILL_3,
            EA_RELOAD,
            EA_UNHOLSTER,
            EA_SPEAK,
            EA_SPEAK_SHORT,
            EA_HEAL_LIGHT_1,
            EA_HIT_ALARM,
            
            EA_COUNT,
        };
        
        CEntityAction(EAction Action):
            m_Action(Action) {}
        
        const char *GetName() const
        {
            const char *ActionNames[] = {
                "corpse_drop",
                "corpse_carry",
                "fire_stand",
                "alt_fire_stand",
                "fire_crouch",
                "death_generic",
                "death_blast_forward",
                "death_blast_backward",
                "death_head_forward",
                "death_head_backward",
                "death_head_neutral",
                "death_chest_forward",
                "death_chest_backward",
                "death_chest_neutral",
                "death_leg_left",
                "death_leg_right",
                "death_crouch",
                "sidestep_left",
                "sidestep_right",
                "roll_left",
                "roll_right",
                "land",
                "flinch_stand",
                "flinch_attack_stand",
                "flinch_chest",
                "flinch_back",
                "flinch_leg_left",
                "flinch_leg_right",
                "idle_to_ready",
                "ready_to_idle",
                "idle_1",
                "idle_2",
                "idle_3",
                "idle_4",
                "rock_drop",
                "rock_pickup",
                "death_still_1",
                "death_still_2",
                "death_still_3",
                "reload",
                "unholster",
                "speak",
                "speak_short",
                "heal_light_1",
                "hit_alarm",
            };
            
            //assert(EA_COUNT == COUNTOF(ActionNames));
            assert((int)m_Action < COUNTOF(ActionNames));
            return ActionNames[(int)m_Action];
        }
    
    private:
        EAction m_Action;
};

#endif // CENTITYACTION_H
