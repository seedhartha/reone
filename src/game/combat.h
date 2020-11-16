/*
 * Copyright (c) 2020 uwadmin12
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <unordered_set>
#include <map>

#include "faction.h"
#include "effect.h"
#include "types.h"
#include "object/creature.h"

#include "SDL2/SDL_timer.h"

#include "../common/timer.h"


namespace reone {

namespace game {

class Area;
class Party;

constexpr float MAX_DETECT_RANGE = 20;              // TODO: adjust detection distance
constexpr uint32_t DEACTIVATION_TIMEOUT = 10000;    // 10s in ticks
constexpr uint32_t AI_MASTER_INTERVAL = 3000;       //3s in ticks

class Combat {

public:
    Combat(Area *area, Party *party);

    /*
    * Always:
    * 0. Update Timers
    * 1. Activity Scanner
    * 2. Sync Effect

    * If Combat Mode Activated:
    * 3. AIMaster
    * 4. Update CombatStateMachine
    * 5. Sync Animation
    */
    void update();

    /*
    * Roles:
    * 1. Scan the surrounding of one combatant per frame, remove the combatant if
    *    no enemies are in visible range & no action in actionQueue
    * 2. Add creatures to _activeCombatants, if applicable
    * 3. Activate/Deactivate global combat mode for party->player
    */
    void activityScanner();

    /*
    * Roles:
    * 1. Evaluate damage/effects
    * 2. Animate damage statistics
    * 3. Feedback Text
    */
    void effectSync();

    /*
    * Roles:
    * 1. Queue Commands (e.g. go to, item consumption, equipment swapping etc.)
    */
    void AIMaster(std::shared_ptr<Creature> &combatant);

    /*
    * Roles:
    * 1. Timed Animation Interrupt Control
    * 2. Combat signal exchange and synchronization
    */
    void combatStateMachine(std::shared_ptr<Creature> &combatant);

    /*
    * Roles:
    * 1. Synchronize dueling and isolated attacks
    * 2. Animate knockdown/whirlwind effects, etc.
    */
    void animationSync();

    std::shared_ptr<Creature> findNearestHostile(std::shared_ptr<Creature> &combatant,
                                                 float detectionRange = MAX_DETECT_RANGE);

    bool activated() { return _activated; }

private:
    Area *_area;
    Party *_party;

    /* combat mode */
    bool _activated = false;

    /* register to _activeCombatants */
    bool registerCombatant(const std::shared_ptr<Creature>& combatant);

    std::deque<std::shared_ptr<Creature>> _activeCombatants;
    std::unordered_set<uint32_t> _activeCombatantIds;

    /* queue[ pair( attacker, victim ) ] */
    std::deque<std::pair<std::shared_ptr<Creature>,
        std::shared_ptr<Creature>>> _duelQueue;
    std::deque<std::pair<std::shared_ptr<Creature>,
        std::shared_ptr<Creature>>> _bashQueue;

    // timers

    /* to be called once each frame, as soon as possible */
    void updateTimers(uint32_t currentTicks);

    void setStateTimeout(const std::shared_ptr<Creature>& creature, uint32_t delayTicks);

    bool isStateTimerDone(const std::shared_ptr<Creature>& creature);

    /* structure: { id : #repetition } */
    std::unordered_map<uint32_t, int> _pendingStates;
    TimerQueue<uint32_t> _stateTimer; // use creature_id

    void setDelayEffectTimeout(std::unique_ptr<Effect>&& eff, std::shared_ptr<Creature>& target, uint32_t delayTicks);

    /*
    * delay the effect application on creature,
    * structure: [ (creature, effect) ]
    */
    std::list<std::pair<std::shared_ptr<Creature>,
                        std::unique_ptr<Effect>>> _effectDelayIndex;
    TimerQueue<decltype(_effectDelayIndex.begin())> _effectDelayTimer;

    TimerMap<uint32_t> _deactivationTimer;

    void setAITimeout(const std::shared_ptr<Creature>& creature);

    bool isAITimerDone(const std::shared_ptr<Creature>& creature);

    std::unordered_set<uint32_t> _pendingAITimer;
    TimerQueue<uint32_t> _AITimer;

    // END timers

    // state transition

    /*
    * Assuming target in range.
    * Queue duel/bash animation
    * Queue delayed effects
    * Set State Timer
    */
    void onEnterAttackState(std::shared_ptr<Creature>& combatant);

    /* Set State Timer */
    void onEnterDefenseState(std::shared_ptr<Creature>& combatant);

    /* Set State Timer */
    void onEnterCooldownState(std::shared_ptr<Creature>& combatant);

    // END state transition

    // activation
    
    void onActivation();
    void onDeactivation();

    // END activation

};

} // namespace game

} // namespace reone
