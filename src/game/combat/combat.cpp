/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "../../common/log.h"
#include "../../common/random.h"

#include "../game.h"

using namespace std;

using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kRoundDuration = 3.0f;
static constexpr float kProjectileDelay = 0.5f;
static constexpr float kDeactivateDelay = 8.0f;

Combat::Combat(Game *game) : _game(game) {
    if (!game) {
        throw invalid_argument("game must not be null");
    }
}

static unique_ptr<Combat::Attack> makeAttack(shared_ptr<Creature> attacker, shared_ptr<SpatialObject> target, shared_ptr<Action> action, AttackResultType resultType, int damage) {
    auto attack = make_unique<Combat::Attack>();
    attack->attacker = move(attacker);
    attack->target = move(target);
    attack->action = move(action);
    attack->resultType = resultType;
    attack->damage = damage;
    return move(attack);
}

static bool isRoundPastFirstAttack(float time) {
    return time >= 0.5f * kRoundDuration;
}

void Combat::addAttack(shared_ptr<Creature> attacker, shared_ptr<SpatialObject> target, shared_ptr<Action> action, AttackResultType resultType, int damage) {
    RoundMap::iterator maybeRound;

    // If attacker has already started a combat round, do nothing
    maybeRound = _roundByAttacker.find(attacker->id());
    if (maybeRound != _roundByAttacker.end()) return;

    // If there is an incomplete combat round where attacker and target roles are reversed, append to that round
    maybeRound = _roundByAttacker.find(target->id());
    if (maybeRound != _roundByAttacker.end()) {
        Round &round = *maybeRound->second;
        if (round.attack1->target == attacker) {
            if (!round.attack2 && !isRoundPastFirstAttack(round.time)) {
                round.attack2 = makeAttack(attacker, target, action, resultType, damage);
                round.duel = true;
                debug(boost::format("Combat: append attack: %s -> %s") % attacker->tag() % target->tag(), 1, DebugChannels::combat);
            }
            return;
        }
    }

    // Otherwise, start a new combat round
    auto round = make_unique<Round>();
    round->attack1 = makeAttack(attacker, target, action, resultType, damage);
    _roundByAttacker.insert(make_pair(attacker->id(), move(round)));
    debug(boost::format("Combat: start round: %s -> %s") % attacker->tag() % target->tag(), 1, DebugChannels::combat);
}

void Combat::update(float dt) {
    for (auto it = _roundByAttacker.begin(); it != _roundByAttacker.end(); ++it) {
        Round &round = *it->second;
        updateRound(round, dt);

        // Remove finished combat rounds
        if (round.state == RoundState::Finished) {
            it = _roundByAttacker.erase(it);
        }
    }
}

void Combat::updateRound(Round &round, float dt) {
    round.time = glm::min(round.time + dt, kRoundDuration);

    switch (round.state) {
        case RoundState::Started: {
            startAttack(*round.attack1, round.duel);
            round.state = RoundState::FirstAttack;
            break;
        }
        case RoundState::FirstAttack: {
            if (isRoundPastFirstAttack(round.time)) {
                resetProjectile(round);
                applyAttackResult(*round.attack1);
                if (round.attack2) {
                    startAttack(*round.attack2, round.duel);
                }
                round.state = RoundState::SecondAttack;
            } else if (round.time >= kProjectileDelay) {
                if (round.projectile) {
                    updateProjectile(round, dt);
                } else {
                    fireProjectile(round.attack1->attacker, round.attack1->target, round);
                }
            }
            break;
        }
        case RoundState::SecondAttack: {
            if (round.time == kRoundDuration) {
                resetProjectile(round);
                if (round.attack2) {
                    applyAttackResult(*round.attack2);
                }
                finishRound(round);
                round.state = RoundState::Finished;
            } else if (round.attack2 && round.time >= 0.5f * kRoundDuration + kProjectileDelay) {
                if (round.projectile) {
                    updateProjectile(round, dt);
                } else {
                    fireProjectile(round.attack2->attacker, round.attack2->target, round);
                }
            }
            break;
        }
        default:
            break;
    }
}

void Combat::startAttack(Attack &attack, bool duel) {
    if (attack.resultType == AttackResultType::Invalid) {
        attack.resultType = determineAttackResult();
    }
    AttackAnimation animation = determineAttackAnimation(attack, duel);

    attack.attacker->face(*attack.target);
    attack.attacker->setMovementType(Creature::MovementType::None);
    attack.attacker->setMovementRestricted(true);
    attack.attacker->playAnimation(animation.attackerAnimation, animation.attackerWieldType, animation.animationVariant);

    if (duel) {
        auto target = static_pointer_cast<Creature>(attack.target);
        target->face(*attack.attacker);
        target->setMovementType(Creature::MovementType::None);
        target->setMovementRestricted(true);
        target->playAnimation(animation.targetAnimation, animation.attackerWieldType, animation.animationVariant);
    }
}

static void finishAttack(Combat::Attack &attack) {
    attack.attacker->deactivateCombat(kDeactivateDelay);
    attack.attacker->setMovementRestricted(false);
    attack.attacker->runEndRoundScript();
    if (attack.action) {
        attack.action->complete();
    }
}

void Combat::finishRound(Round &round) {
    finishAttack(*round.attack1);
    if (round.attack2) {
        finishAttack(*round.attack2);
    }
    round.state = RoundState::Finished;
    debug(boost::format("Combat: finish round: %s -> %s") % round.attack1->attacker->tag() % round.attack1->target->tag(), 1, DebugChannels::combat);
}

} // namespace game

} // namespace reone
