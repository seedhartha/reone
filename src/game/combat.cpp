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

#include <algorithm>
#include <climits>

#include "glm/common.hpp"

#include "../common/log.h"

#include "game.h"

using namespace std;

namespace reone {

namespace game {

constexpr float kRoundDuration = 3.0f;

static AttackAction *getAttackAction(const shared_ptr<Creature> &combatant) {
    return dynamic_cast<AttackAction *>(combatant->actionQueue().currentAction());
}

void Combat::Round::advance(float dt) {
    time = glm::min(time + dt, kRoundDuration);
}

Combat::Combat(Game *game) : _game(game) {
    if (!game) {
        throw invalid_argument("game must not be null");
    }
}

void Combat::update(float dt) {
    _heartbeatTimer.update(dt);

    if (_heartbeatTimer.hasTimedOut()) {
        _heartbeatTimer.reset(kHeartbeatInterval);
        updateCombatants();
        updateAI();
    }
    updateRounds(dt);
    updateActivation();
}

void Combat::updateCombatants() {
    ObjectList &creatures = _game->module()->area()->getObjectsByType(ObjectType::Creature);
    for (auto &object : creatures) {
        shared_ptr<Creature> creature(static_pointer_cast<Creature>(object));

        vector<shared_ptr<Creature>> enemies(getEnemies(*creature));
        bool hasEnemies = !enemies.empty();

        auto maybeCombatant = _combatantById.find(creature->id());
        if (maybeCombatant != _combatantById.end()) {
            if (hasEnemies) {
                maybeCombatant->second->enemies = move(enemies);
            } else {
                _combatantById.erase(maybeCombatant);
                creature->setCombat(false);
            }
            continue;
        }
        if (hasEnemies) {
            auto combatant = make_shared<Combatant>();
            combatant->creature = creature;
            combatant->enemies = move(enemies);
            _combatantById.insert(make_pair(creature->id(), move(combatant)));
            creature->setCombat(true);
        }
    }
}

void Combat::updateAI() {
    for (auto &pair : _combatantById) {
        updateCombatantAI(*pair.second);
    }
}

void Combat::updateCombatantAI(Combatant &combatant) {
    shared_ptr<Creature> creature(combatant.creature);

    shared_ptr<Creature> enemy(getNearestEnemy(combatant));
    if (!enemy) return;

    AttackAction *action = getAttackAction(creature);
    if (action && action->target() == enemy) return;

    ActionQueue &actions = creature->actionQueue();
    actions.clear();
    actions.add(make_unique<AttackAction>(enemy, creature->attackRange()));

    debug(boost::format("Combat: attack action added: '%s' -> '%s'") % creature->tag() % enemy->tag(), 2);
}

shared_ptr<Creature> Combat::getNearestEnemy(const Combatant &combatant) const {
    shared_ptr<Creature> result;
    float minDist = FLT_MAX;

    for (auto &enemy : combatant.enemies) {
        float dist = enemy->distanceTo(*combatant.creature);
        if (dist >= minDist) continue;

        result = enemy;
        minDist = dist;
    }

    return move(result);
}

vector<shared_ptr<Creature>> Combat::getEnemies(const Creature &combatant, float range) const {
    vector<shared_ptr<Creature>> result;

    ObjectList creatures(_game->module()->area()->getObjectsByType(ObjectType::Creature));
    for (auto &object : creatures) {
        if (object->distanceTo(combatant) > range) continue;

        shared_ptr<Creature> creature(static_pointer_cast<Creature>(object));
        if (!getIsEnemy(combatant, *creature)) continue;

        // TODO: check line-of-sight

        result.push_back(move(creature));
    }

    return move(result);
}

void Combat::updateRounds(float dt) {
    for (auto &pair : _combatantById) {
        shared_ptr<Combatant> attacker(pair.second);

        // Do not start a combat round, if attacker is a moving party leader

        if (attacker->creature->id() == _game->party().leader()->id() && _game->module()->player().isMovementRequested()) continue;

        // Check if attacker already participates in a combat round

        auto maybeAttackerRound = _roundByAttackerId.find(attacker->creature->id());
        if (maybeAttackerRound != _roundByAttackerId.end()) continue;

        // Check if attacker is close enough to attack its target

        AttackAction *action = getAttackAction(attacker->creature);
        if (!action) continue;

        shared_ptr<Creature> defender(action->target());
        if (!defender || defender->distanceTo(*attacker->creature) > action->range()) continue;

        // Check if target is valid combatant

        auto maybeDefender = _combatantById.find(defender->id());
        if (maybeDefender == _combatantById.end()) continue;

        attacker->target = defender;

        // Create a combat round if not a duel

        auto maybeDefenderRound = _roundByAttackerId.find(defender->id());
        bool isDuel = maybeDefenderRound != _roundByAttackerId.end() && maybeDefenderRound->second->defender == attacker;

        if (!isDuel) {
            auto round = make_shared<Round>();
            round->attacker = attacker;
            round->defender = maybeDefender->second;
            _roundByAttackerId.insert(make_pair(attacker->creature->id(), round));
        }
    }
    for (auto it = _roundByAttackerId.begin(); it != _roundByAttackerId.end(); ) {
        shared_ptr<Round> round(it->second);
        updateRound(*round, dt);

        if (round->state == RoundState::Finished) {
            round->attacker->target.reset();
            it = _roundByAttackerId.erase(it);
        } else {
            ++it;
        }
    }
}

void Combat::updateRound(Round &round, float dt) {
    round.advance(dt);

    shared_ptr<Creature> attacker(round.attacker->creature);
    shared_ptr<Creature> defender(round.defender->creature);
    bool isDuel = round.defender->target == attacker;

    switch (round.state) {
        case RoundState::Started:
            attacker->face(*defender);
            attacker->setMovementType(Creature::MovementType::None);
            attacker->setMovementRestricted(true);
            if (isDuel) {
                attacker->playAnimation(Creature::Animation::DuelAttack);
                defender->face(*attacker);
                defender->setMovementType(Creature::MovementType::None);
                defender->setMovementRestricted(true);
                defender->playAnimation(Creature::Animation::Dodge);
            } else {
                attacker->playAnimation(Creature::Animation::BashAttack);
            }
            round.state = RoundState::FirstTurn;
            debug(boost::format("Combat: first round turn started: '%s' -> '%s'") % attacker->tag() % defender->tag(), 2);
            break;

        case RoundState::FirstTurn:
            if (round.time >= 0.5f * kRoundDuration) {
                if (isDuel) {
                    defender->face(*attacker);
                    defender->playAnimation(Creature::Animation::DuelAttack);
                    attacker->face(*defender);
                    attacker->playAnimation(Creature::Animation::Dodge);
                }
                round.state = RoundState::SecondTurn;
                debug(boost::format("Combat: second round turn started: '%s' -> '%s'") % attacker->tag() % defender->tag(), 2);
            }
            break;

        case RoundState::SecondTurn:
            if (round.time == kRoundDuration) {
                attacker->setMovementRestricted(false);
                defender->setMovementRestricted(false);
                round.state = RoundState::Finished;
                debug(boost::format("Combat: round finished: '%s' -> '%s'") % attacker->tag() % defender->tag(), 2);
            }
            break;

        default:
            break;
    }
}

void Combat::updateActivation() {
    shared_ptr<Creature> partyLeader(_game->party().leader());
    bool active = partyLeader && _combatantById.count(partyLeader->id()) != 0;
    if (_active == active) return;

    if (active) onEnterCombatMode();
    else onExitCombatMode();

    _active = active;
}

bool Combat::isActive() const {
    return _active;
}

void Combat::onEnterCombatMode() {
    _game->module()->area()->setCombatTPCamera();
}

void Combat::onExitCombatMode() {
    _game->module()->area()->setDefaultTPCamera();
}

} // namespace game

} // namespace reone
