/*
 * Copyright (c) 2020 The reone project contributors
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

#include "combat.h"

#include "../common/log.h"

#include "object/area.h"
#include "party.h"

using namespace std;

namespace reone {

namespace game {

// Helper functions

static AttackAction *getAttackAction(const shared_ptr<Creature> &combatant) {
    return dynamic_cast<AttackAction *>(combatant->actionQueue().currentAction());
}

static bool isActiveTargetInRange(const shared_ptr<Creature> &combatant) {
    auto* action = getAttackAction(combatant);
    return action && action->isInRange();
}

static void duel(const shared_ptr<Creature> &attacker, const shared_ptr<Creature> &target) {
    target->face(*attacker);
    attacker->face(*target);
    attacker->playAnimation(Creature::Animation::UnarmedAttack1);
    target->playAnimation(Creature::Animation::UnarmedDodge1);
}

static void bash(const shared_ptr<Creature> &attacker, const shared_ptr<Creature> &target) {
    attacker->face(*target);
    attacker->playAnimation(Creature::Animation::UnarmedAttack2);
}

static void flinch(const shared_ptr<Creature> &target) {
    target->playAnimation(Creature::Animation::Flinch);
}

// END Helper functions

Combat::Combat(Area *area, Party *party) : _area(area), _party(party) {
    if (!area) {
        throw invalid_argument("area must not be null");
    }
}

void Combat::update() {
    updateTimers(SDL_GetTicks());

    shared_ptr<Creature> partyLeader(_party->leader());
    if (partyLeader) {
        scanHostility(partyLeader);
    }
    activityScanner();

    if (isActivated()) {
        // One AIMaster per frame, rotated by activityScanner
        if (isAITimerDone(_activeCombatants.front())) {
            AIMaster(_activeCombatants.front());
            setAITimeout(_activeCombatants.front());
        }

        for (auto &cbt : _activeCombatants) {
            combatStateMachine(cbt);
        }
        animationSync();

        // rotate _activeCombatants
        _activeCombatants.push_back(_activeCombatants.front());
        _activeCombatants.pop_front();
    }
}

void Combat::updateTimers(uint32_t currentTicks) {
    _stateTimers.update(currentTicks);
    _effectDelayTimers.update(currentTicks);
    _deactivationTimers.update(currentTicks);
    _aiTimers.update(currentTicks);

    for (const auto &id : _stateTimers.completed) {
        if (--(_pendingStates[id]) == 0)
            _pendingStates.erase(id);
    }
    _stateTimers.completed.clear();

    for (auto &pr : _effectDelayTimers.completed) {
        if (!pr->first) continue;

        pr->first->applyEffect(move(pr->second));
        pr->second = nullptr; // dangling?
        _effectDelayIndex.erase(pr);
    }
    _effectDelayTimers.completed.clear();

    for (auto &id : _aiTimers.completed) {
        _pendingAITimers.erase(id);
    }
    _aiTimers.completed.clear();
}

bool Combat::scanHostility(const shared_ptr<Creature> &subject) {
    bool stillActive = false;

    ObjectList creatures(_area->getObjectsByType(ObjectType::Creature));
    for (auto &object : creatures) {
        if (object->distanceTo(*subject) > kDetectionRange) continue;

        shared_ptr<Creature> creature(static_pointer_cast<Creature>(object));
        if (!getIsEnemy(*subject, *creature)) continue;

        stillActive = true;

        if (registerCombatant(creature)) { // will fail if already registered
            debug(boost::format("combat: registered '%s', faction '%d'") % creature->tag() % static_cast<int>(creature->faction()));
        }

        // TODO: add line-of-sight requirement
    }

    return stillActive;
}

void Combat::activityScanner() {
    if (_activeCombatants.empty()) return;

    shared_ptr<Creature> actor(_activeCombatants.front());
    bool stillActive = scanHostility(actor);

    // deactivate actor if !active
    if (!stillActive) { //&& getAttackAction(actor) == nullptr ???
        if (_deactivationTimers.completed.count(actor->id()) == 1) {
            _deactivationTimers.completed.erase(actor->id());

            // deactivation timeout complete
            actor->setCombatState(CombatState::Idle);

            _activeCombatants.pop_front();
            _activeCombatantIds.erase(actor->id());

            debug(boost::format("combat: deactivated '%s', combat_mode[%d]") % actor->tag() % isActivated());
        }
        else if (!_deactivationTimers.isRegistered(actor->id())) {
            _deactivationTimers.setTimeout(actor->id(), kDeactivationTimeout);

            debug(boost::format("combat: registered deactivation timer'%s'") % actor->tag());
        }
    }
    else {
        if (_deactivationTimers.isRegistered(actor->id())) {
            _deactivationTimers.cancel(actor->id());

            debug(boost::format("combat: cancelled deactivation timer'%s'") % actor->tag());
        }
    }
}

void Combat::effectSync() {
}

void Combat::AIMaster(const shared_ptr<Creature> &combatant) {
    if (combatant->id() == _party->leader()->id()) return;

    ActionQueue &cbt_queue = combatant->actionQueue();

    //if (cbt_queue.currentAction()) return;

    auto hostile = findNearestHostile(combatant, kDetectionRange);
    if (hostile) {
        cbt_queue.add(make_unique<AttackAction>(hostile));
        debug(boost::format("AIMaster: '%s' Queued to attack '%s'") % combatant->tag()
            % hostile->tag());
    }
}

void Combat::setStateTimeout(const shared_ptr<Creature> &creature, uint32_t delayTicks) {
    if (!creature) return;

    _stateTimers.setTimeout(creature->id(), delayTicks);

    // in case of repetition
    if (_pendingStates.count(creature->id()) == 0) {
        _pendingStates[creature->id()] = 0;
    }
    ++(_pendingStates[creature->id()]);
}

bool Combat::isStateTimerDone(const shared_ptr<Creature> &creature) {
    if (!creature) return false;

    return _pendingStates.count(creature->id()) == 0;
}

void Combat::setDelayEffectTimeout(
    unique_ptr<Effect> &&eff,
    const shared_ptr<Creature> &target,
    uint32_t delayTicks
) {
    auto index = _effectDelayIndex.insert(
        _effectDelayIndex.end(),
        make_pair(target, move(eff)));

    _effectDelayTimers.setTimeout(index, delayTicks);
}

void Combat::setAITimeout(const shared_ptr<Creature> &creature) {
    if (!creature) return;

    _aiTimers.setTimeout(creature->id(), kAIMasterInterval);
    _pendingAITimers.insert(creature->id());
}

bool Combat::isAITimerDone(const shared_ptr<Creature> &creature) {
    if (!creature) return false;

    return _pendingAITimers.count(creature->id()) == 0;
}

void Combat::onEnterAttackState(const shared_ptr<Creature> &combatant) {
    if (!combatant) return;

    setStateTimeout(combatant, 1500);
    debug(boost::format("'%s' enters Attack state, actionQueueLen[%d], attackAction[%d]")
        % combatant->tag() % combatant->actionQueue().size()
        % (getAttackAction(combatant) != nullptr)); // TODO: disable redundant info

    AttackAction *action = getAttackAction(combatant);
    shared_ptr<Creature> target(action->target());

    if (target && (target->combatState() == CombatState::Idle || target->combatState() == CombatState::Cooldown)) {
        _duelQueue.push_back(make_pair(combatant, target));

        // synchronization
        combatStateMachine(target);
    } else {
        _bashQueue.push_back(make_pair(combatant, target));
    }

    setDelayEffectTimeout(
        make_unique<DamageEffect>(combatant), target, 500 // dummy delay
    );

    action->complete();
}

void Combat::onEnterDefenseState(const shared_ptr<Creature> &combatant) {
    setStateTimeout(combatant, 1500);
    debug(boost::format("'%s' enters Defense state, set_timer") % combatant->tag());
}

void Combat::onEnterCooldownState(const shared_ptr<Creature> &combatant) {
    setStateTimeout(combatant, 1500);
    debug(boost::format("'%s' enters Cooldown state, set_timer") % combatant->tag());
}

void Combat::combatStateMachine(const shared_ptr<Creature> &combatant) {
    switch (combatant->combatState()) {
    case CombatState::Idle:
        for (auto &pr : _duelQueue) { // if combatant is caught in a duel
            if (pr.second && pr.second->id() == combatant->id()) {
                combatant->setCombatState(CombatState::Defense);
                onEnterDefenseState(combatant);
                return;
            }
        }

        if (isActiveTargetInRange(combatant)) {
            combatant->setCombatState(CombatState::Attack);
            onEnterAttackState(combatant);
        }
        return;

    case CombatState::Attack:
        if (isStateTimerDone(combatant)) {
            combatant->setCombatState(CombatState::Cooldown);
            onEnterCooldownState(combatant);
        }
        return;

    case CombatState::Cooldown:
        for (auto &pr : _duelQueue) { // if combatant is caught in a duel
            if (pr.second && pr.second->id() == combatant->id()) {
                combatant->setCombatState(CombatState::Defense);
                onEnterDefenseState(combatant);
                return;
            }
        }

        if (isStateTimerDone(combatant)) {
            combatant->setCombatState(CombatState::Idle);
            debug(boost::format("'%s' enters Idle state") % combatant->tag());
        }
        return;

    case CombatState::Defense:
        if (isStateTimerDone(combatant)) {
            combatant->setCombatState(CombatState::Idle);
            debug(boost::format("'%s' enters Idle state") % combatant->tag());

            // synchronization
            combatStateMachine(combatant);
        }
        return;

    default:
        return;
    }
}

void Combat::animationSync() {
    while (!_duelQueue.empty()) {
        auto &pr = _duelQueue.front();
        duel(pr.first, pr.second);
        _duelQueue.pop_front();
    }

    while (!_bashQueue.empty()) {
        auto &pr = _bashQueue.front();
        duel(pr.first, pr.second);
        _bashQueue.pop_front();
    }
}

shared_ptr<Creature> Combat::findNearestHostile(const shared_ptr<Creature> &combatant, float detectionRange) {
    shared_ptr<Creature> closest_target = nullptr;
    float min_dist = detectionRange;

    for (auto &creature : _activeCombatants) {
        if (creature->id() == combatant->id()) continue;

        if (!getIsEnemy(static_cast<Creature &>(*creature), *combatant))
            continue;

        float distance = glm::length(creature->position() - combatant->position()); // TODO: fine tune the distance
        if (distance < min_dist) {
            min_dist = distance;
            closest_target = static_pointer_cast<Creature>(creature);
        }
    }

    return move(closest_target);
}

bool Combat::isActivated() const {
    return !_activeCombatants.empty();
}

bool Combat::registerCombatant(const shared_ptr<Creature> &combatant) {
    auto res = _activeCombatantIds.insert(combatant->id());
    if (res.second) { // combatant not already in _activeCombatantIds
        _activeCombatants.push_back(combatant);
    }
    return res.second;
}

} // namespace game

} // namespace reone
