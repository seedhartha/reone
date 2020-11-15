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

namespace reone {

namespace game {

class Area;
class Party;

template <typename T>
struct TimerPairCompare {
    constexpr bool operator()(
        std::pair<uint32_t, T> const& a,
        std::pair<uint32_t, T> const& b)
        const noexcept {
        return a.first > b.first;
    } // min heap!
};

template <typename T>
struct TimerQueue {
    void setTimeout(const T &obj, uint32_t tick) {
        _timer.push(std::make_pair(_timestamp + tick, obj));
    }

    /* call once per frame, as soon as possible */
    void update(uint32_t currentTicks) {
        _timestamp = currentTicks;

        while (!_timer.empty() && _timer.top().first < _timestamp) {
            completed.push_back(_timer.top().second);
            _timer.pop();
        }
    }

    /* users are responsible for managing this */
    std::list<T> completed;

private:
    std::priority_queue<std::pair<uint32_t, T>,
        std::vector<std::pair<uint32_t, T>>,
        TimerPairCompare<T>> _timer;

    uint32_t _timestamp;
};

constexpr float MAX_DETECT_RANGE = 20; // TODO: adjust detection distance

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

    std::shared_ptr<Creature> findNearestHostile(std::shared_ptr<Creature> &combatant);

private:
    Area *_area;
    Party *_party;

    /* combat mode */
    bool _activated = false;

    /* register to _activeCombatants */
    bool registerCombatant(const std::shared_ptr<Creature> &combatant) {
        auto res = _activeCombatantIds.insert(combatant->id());
        if (res.second) { // combatant not already in _activeCombatantIds
            _activeCombatants.push_back(combatant);
        }
        return res.second;
    }

    std::deque<std::shared_ptr<Creature>> _activeCombatants;
    std::unordered_set<uint32_t> _activeCombatantIds;

    /* queue[ pair( attacker, victim ) ] */
    std::deque<std::pair<std::shared_ptr<Creature>,
        std::shared_ptr<Creature>>> _duelQueue;
    std::deque<std::pair<std::shared_ptr<Creature>,
        std::shared_ptr<Creature>>> _bashQueue;

    // timers

    /* to be called once each frame, as soon as possible */
    void updateTimers(uint32_t currentTicks) {
        _stateTimer.update(currentTicks);
        _effectDelayTimer.update(currentTicks);

        for (const auto& e : _stateTimer.completed) {
            if (--(_pendingStates[e]) == 0)
                _pendingStates.erase(e);
        }
        _stateTimer.completed.clear();

        for (auto& pr : _effectDelayTimer.completed) {
            if (!pr->first) continue;

            pr->first->applyEffect(std::move(pr->second));
            pr->second = nullptr; // dangling?
            _effectDelayIndex.erase(pr);
        }
        _effectDelayTimer.completed.clear();
    }

    void setStateTimeout(const std::shared_ptr<Creature>& creature, uint32_t delayTicks) {
        if (!creature) return;
        _stateTimer.setTimeout(creature->id(), delayTicks);

        // in case of repetition
        if (_pendingStates.count(creature->id()) == 0) _pendingStates[creature->id()] = 0;
        ++(_pendingStates[creature->id()]);
    }

    bool isStateTimerDone(const std::shared_ptr<Creature>& creature) {
        if (!creature) return false;
        return _pendingStates.count(creature->id()) == 0;
    }

    /* structure: { id : #repetition } */
    std::unordered_map<uint32_t, int> _pendingStates;
    TimerQueue<uint32_t> _stateTimer; // use creature_id

    void setDelayEffectTimeout(std::unique_ptr<Effect> && eff, std::shared_ptr<Creature> & target, uint32_t delayTicks) {
        auto index = _effectDelayIndex.insert(_effectDelayIndex.end(), std::make_pair(target, std::move(eff)));
        _effectDelayTimer.setTimeout(index, delayTicks);
    }

    /*
    * delay the effect application on creature,
    * structure: [ (creature, effect) ]
    */
    std::list<std::pair<std::shared_ptr<Creature>,
                        std::unique_ptr<Effect>>> _effectDelayIndex;
    TimerQueue<decltype(_effectDelayIndex.begin())> _effectDelayTimer;

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

};

} // namespace game

} // namespace reone
