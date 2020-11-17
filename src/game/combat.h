/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <map>
#include <unordered_set>

#include "SDL2/SDL_timer.h"

#include "../common/timermap.h"
#include "../common/timerqueue.h"

#include "enginetype/effect.h"
#include "object/creature.h"
#include "rp/factions.h"
#include "types.h"

namespace reone {

namespace game {

constexpr float kDetectionRange = 20.0f;
constexpr uint32_t kDeactivationTimeout = 10000; // 10s in ticks
constexpr uint32_t kAIMasterInterval = 3000; // 3s in ticks

class Area;
class Party;

class Combat {
public:
    Combat(Area *area, Party *party);

    /**
     * Always:
     * 1. Update Timers
     * 2. Activity Scanner
     * 3. Sync Effect
     *
     * If Combat Mode Activated:
     * 4. AIMaster
     * 5. Update CombatStateMachine
     * 6. Sync Animation
     */
    void update();

    /**
     * Roles:
     * 1. Scan the first of activeCombatants each frame, remove the combatant
     *    if no enemies are in visible range & no action in actionQueue
     * 2. Add creatures to _activeCombatants, if applicable
     * 3. Activate/Deactivate global combat mode for party->player
     */
    void activityScanner();

    /**
     * Roles:
     * 1. Evaluate damage/effects
     * 2. Animate damage statistics
     * 3. Feedback Text
     */
    void effectSync();

    /**
     * Roles:
     * 1. Queue Commands (e.g. go to, item consumption, equipment swapping etc.)
     */
    void AIMaster(const std::shared_ptr<Creature> &combatant);

    /**
     * Roles:
     * 1. Timed Animation Interrupt Control
     * 2. Combat signal exchange and synchronization
     */
    void combatStateMachine(const std::shared_ptr<Creature> &combatant);

    /**
     * Roles:
     * 1. Synchronize dueling and isolated attacks
     * 2. Animate knockdown/whirlwind effects, etc.
     */
    void animationSync();

    std::shared_ptr<Creature> findNearestHostile(const std::shared_ptr<Creature> &combatant,
                                                 float detectionRange = kDetectionRange);

    bool activated() { return !_activeCombatants.empty(); }

private:
    Area *_area;
    Party *_party;

    /* register to _activeCombatants */
    bool registerCombatant(const std::shared_ptr<Creature> &combatant);

    /* register hostiles to activecombatant list, return stillActive */
    bool scanHostility(const std::shared_ptr<Creature> &combatant);

    std::deque<std::shared_ptr<Creature>> _activeCombatants;
    std::unordered_set<uint32_t> _activeCombatantIds;

    /* queue[ pair( attacker, victim ) ] */
    std::deque<std::pair<std::shared_ptr<Creature>, std::shared_ptr<Creature>>> _duelQueue;
    std::deque<std::pair<std::shared_ptr<Creature>, std::shared_ptr<Creature>>> _bashQueue;

    // Timers

    /* to be called once each frame, as soon as possible */
    void updateTimers(uint32_t currentTicks);

    void setStateTimeout(const std::shared_ptr<Creature> &creature, uint32_t delayTicks);

    bool isStateTimerDone(const std::shared_ptr<Creature> &creature);

    /* structure: { id : #repetition } */
    std::unordered_map<uint32_t, int> _pendingStates;
    TimerQueue<uint32_t> _stateTimers; // use creature_id

    void setDelayEffectTimeout(std::unique_ptr<Effect> &&eff, const std::shared_ptr<Creature> &target, uint32_t delayTicks);

    /*
     * delay the effect application on creature,
     * structure: [ (creature, effect) ]
     */
    std::list<std::pair<std::shared_ptr<Creature>,
                        std::unique_ptr<Effect>>> _effectDelayIndex;

    TimerQueue<decltype(_effectDelayIndex.begin())> _effectDelayTimers;

    TimerMap<uint32_t> _deactivationTimers;

    void setAITimeout(const std::shared_ptr<Creature> &creature);

    bool isAITimerDone(const std::shared_ptr<Creature>& creature);

    std::unordered_set<uint32_t> _pendingAITimers;
    TimerQueue<uint32_t> _aiTimers;

    // END Timers

    // State transition

    /**
     * Assuming target is in range:
     * 1. Queue duel/bash animation
     * 2. Queue delayed effects
     * 3. Set State Timer
     */
    void onEnterAttackState(const std::shared_ptr<Creature> &combatant);

    /* Set State Timer */
    void onEnterDefenseState(const std::shared_ptr<Creature> &combatant);

    /* Set State Timer */
    void onEnterCooldownState(const std::shared_ptr<Creature> &combatant);

    // END State transition

};

} // namespace game

} // namespace reone
