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

#include <algorithm>
#include <climits>

#include "glm/common.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "../../common/log.h"
#include "../../common/random.h"

#include "../game.h"
#include "../rp/factionutil.h"

using namespace std;

using namespace reone::render;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kRoundDuration = 3.0f;

static bool g_projectilesEnabled = true;

static shared_ptr<AttackAction> getAttackAction(const shared_ptr<Creature> &combatant) {
    return dynamic_pointer_cast<AttackAction>(combatant->actionQueue().getCurrentAction());
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
    if (_heartbeatTimer.advance(dt)) {
        updateCombatants();
        updateAI();
        _heartbeatTimer.reset(kHeartbeatInterval);
    }
    updateRounds(dt);
    updateActivation();
}

void Combat::updateCombatants() {
    for (auto &object : _game->module()->area()->getObjectsByType(ObjectType::Creature)) {
        auto creature = static_pointer_cast<Creature>(object);
        if (!creature->isDead()) {
            updateCombatant(creature);
        }
    }
    removeStaleCombatants();
}

void Combat::updateCombatant(const shared_ptr<Creature> &creature) {
    vector<shared_ptr<Creature>> enemies(getEnemies(*creature));

    auto maybeCombatant = _combatantById.find(creature->id());
    if (maybeCombatant != _combatantById.end()) {
        maybeCombatant->second->enemies = move(enemies);
        return;
    }

    if (!enemies.empty()) {
        addCombatant(creature, move(enemies));
    }
}

vector<shared_ptr<Creature>> Combat::getEnemies(const Creature &combatant, float range) const {
    vector<shared_ptr<Creature>> result;
    shared_ptr<Area> area(_game->module()->area());

    for (auto &object : area->getObjectsByType(ObjectType::Creature)) {
        if (object.get() == &combatant ||
            object->isDead() ||
            object->distanceTo(combatant) > range) continue;

        auto creature = static_pointer_cast<Creature>(object);
        if (!getIsEnemy(combatant, *creature)) continue;

        glm::vec3 adjustedCombatantPos(combatant.position());
        adjustedCombatantPos.z += 1.8f; // TODO: height based on appearance

        glm::vec3 adjustedCreaturePos(creature->position());
        adjustedCreaturePos.z += creature->model()->aabb().center().z;

        glm::vec3 combatantToCreature(adjustedCreaturePos - adjustedCombatantPos);

        RaycastProperties castProps;
        castProps.flags = kRaycastRooms | kRaycastObjects | kRaycastAABB;
        castProps.objectTypes = { ObjectType::Door };
        castProps.origin = adjustedCombatantPos;
        castProps.direction = glm::normalize(combatantToCreature);
        castProps.maxDistance = glm::length(combatantToCreature);

        RaycastResult castResult;

        const CollisionDetector &detector = area->collisionDetector();
        if (detector.raycast(castProps, castResult)) continue;

        // TODO: check field of view

        result.push_back(move(creature));
    }

    return move(result);
}

shared_ptr<Combat::Combatant> Combat::addCombatant(const shared_ptr<Creature> &creature, EnemiesList enemies) {
    debug(boost::format("Combat: add combatant '%s' with %d enemies") % creature->tag() % enemies.size(), 2);

    creature->setInCombat(true);

    auto combatant = make_shared<Combatant>();
    combatant->creature = creature;
    combatant->enemies = move(enemies);

    _combatantById.insert(make_pair(creature->id(), combatant));

    return move(combatant);
}

void Combat::removeStaleCombatants() {
    for (auto it = _combatantById.begin(); it != _combatantById.end(); ) {
        shared_ptr<Combatant> combatantPtr(it->second);

        // Combatant is considered stale if he is either dead or (has no enemies
        // and is not participating in a combat round)
        bool isStale = combatantPtr->creature->isDead();
        if (!isStale) {
            auto maybeRound = _roundByAttackerId.find(combatantPtr->creature->id());
            if (maybeRound == _roundByAttackerId.end() && combatantPtr->enemies.empty()) {
                isStale = true;
            }
        }
        if (isStale) {
            combatantPtr->creature->setInCombat(false);
            it = _combatantById.erase(it);
        } else {
            ++it;
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

    shared_ptr<AttackAction> action(getAttackAction(creature));
    if (action && action->target() == enemy) return;

    ActionQueue &actions = creature->actionQueue();
    actions.clear();
    actions.add(make_unique<AttackAction>(enemy, creature->getAttackRange()));

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

void Combat::updateRounds(float dt) {
    for (auto &pair : _combatantById) {
        shared_ptr<Combatant> attacker(pair.second);
        if (attacker->creature->isDead()) continue;

        // Do not start a combat round, if attacker is a moving party leader

        if (attacker->creature->id() == _game->party().getLeader()->id() && _game->module()->player().isMovementRequested()) continue;

        // Check if attacker already participates in a combat round

        auto maybeAttackerRound = _roundByAttackerId.find(attacker->creature->id());
        if (maybeAttackerRound != _roundByAttackerId.end()) continue;

        // Check if attacker is close enough to attack its target

        shared_ptr<AttackAction> action(getAttackAction(attacker->creature));
        if (!action) continue;

        shared_ptr<SpatialObject> target(action->target());
        if (!target || target->distanceTo(*attacker->creature) > action->range()) continue;

        attacker->target = target;

        // Create a combat round if not a duel

        auto maybeTargetRound = _roundByAttackerId.find(target->id());
        bool duel = maybeTargetRound != _roundByAttackerId.end() && maybeTargetRound->second->target == attacker->creature;
        if (!duel) {
            addRound(attacker, target);
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

void Combat::addRound(const shared_ptr<Combatant> &attacker, const shared_ptr<SpatialObject> &target) {
    auto round = make_shared<Round>();
    round->attacker = attacker;
    round->target = target;
    addRound(round);
}

void Combat::addRound(const shared_ptr<Round> &round) {
    debug(boost::format("Combat: add round: '%s' -> '%s'") % round->attacker->creature->tag() % round->target->tag(), 2);
    _roundByAttackerId.insert(make_pair(round->attacker->creature->id(), round));
}

void Combat::updateRound(Round &round, float dt) {
    round.advance(dt);

    shared_ptr<Creature> attacker(round.attacker->creature);

    if (attacker->isDead() || round.target->isDead()) {
        finishRound(round);
        return;
    }

    auto maybeTargetCombatant = _combatantById.find(round.target->id());
    bool duel = maybeTargetCombatant != _combatantById.end() && maybeTargetCombatant->second->target == attacker;

    switch (round.state) {
        case RoundState::Started: {
            round.attackResult = _attackResolver.getAttackResult(attacker, round.target, duel, round.cutsceneAttackResult);

            attacker->face(*round.target);
            attacker->setMovementType(Creature::MovementType::None);
            attacker->setMovementRestricted(true);
            attacker->playAnimation(round.attackResult.attackerAnimation, round.attackResult.attackerWieldType, round.attackResult.animationVariant);

            fireProjectile(attacker, round.target, round);

            if (duel) {
                auto targetCreature = static_pointer_cast<Creature>(round.target);
                targetCreature->face(*attacker);
                targetCreature->setMovementType(Creature::MovementType::None);
                targetCreature->setMovementRestricted(true);
                targetCreature->playAnimation(round.attackResult.targetAnimation, round.attackResult.attackerWieldType, round.attackResult.animationVariant);
            }

            round.state = RoundState::FirstTurn;
            debug(boost::format("Combat: first round turn started: '%s' -> '%s'") % attacker->tag() % round.target->tag(), 2);
            break;
        }
        case RoundState::FirstTurn:
            if (round.time >= 0.5f * kRoundDuration) {
                if (round.projectile) {
                    _game->sceneGraph().removeRoot(round.projectile);
                    round.projectile.reset();
                }
                if (round.cutscene) {
                    applyAttackResult(attacker, round.target, round.attackResult, round.cutsceneDamage);
                } else {
                    applyAttackResult(attacker, round.target, round.attackResult);
                }
                if (duel) {
                    auto targetCreature = static_pointer_cast<Creature>(round.target);
                    round.attackResult = _attackResolver.getAttackResult(targetCreature, attacker, true);

                    targetCreature->face(*attacker);
                    targetCreature->playAnimation(round.attackResult.attackerAnimation, round.attackResult.attackerWieldType, round.attackResult.animationVariant);

                    attacker->face(*round.target);
                    attacker->playAnimation(round.attackResult.targetAnimation, round.attackResult.attackerWieldType, round.attackResult.animationVariant);

                    fireProjectile(targetCreature, attacker, round);
                }
                round.state = RoundState::SecondTurn;
                debug(boost::format("Combat: second round turn started: '%s' -> '%s'") % attacker->tag() % round.target->tag(), 2);

            } else {
                updateProjectile(round, dt);
            }
            break;

        case RoundState::SecondTurn:
            if (round.time == kRoundDuration) {
                if (round.projectile) {
                    _game->sceneGraph().removeRoot(round.projectile);
                    round.projectile.reset();
                }
                if (duel) {
                    applyAttackResult(static_pointer_cast<Creature>(round.target), attacker, round.attackResult);
                }
                finishRound(round);
            } else {
                updateProjectile(round, dt);
            }
            break;

        default:
            break;
    }
}

void Combat::finishRound(Round &round) {
    shared_ptr<Creature> attacker(round.attacker->creature);
    attacker->setMovementRestricted(false);

    if (round.target->type() == ObjectType::Creature) {
        auto targetCreature = static_pointer_cast<Creature>(round.target);
        targetCreature->setMovementRestricted(false);
    }

    _game->sceneGraph().removeRoot(round.projectile);
    round.projectile.reset();

    round.state = RoundState::Finished;
    debug(boost::format("Combat: round finished: '%s' -> '%s'") % attacker->tag() % round.target->tag(), 2);
}

void Combat::fireProjectile(const shared_ptr<Creature> &attacker, const shared_ptr<SpatialObject> &target, Round &round) {
    if (!g_projectilesEnabled) return;

    shared_ptr<Item> weapon(attacker->getEquippedItem(InventorySlot::kInventorySlotRightWeapon));
    if (!weapon) return;

    shared_ptr<Item::AmmunitionType> ammunitionType(weapon->ammunitionType());
    if (!ammunitionType) return;

    shared_ptr<ModelSceneNode> weaponModel(attacker->model()->getAttachedModel("rhand"));
    if (!weaponModel) return;

    glm::vec3 projectilePosition, bulletHookPosition;
    if (weaponModel->getNodeAbsolutePosition("bullethook", bulletHookPosition)) {
        projectilePosition = weaponModel->absoluteTransform() * glm::vec4(bulletHookPosition, 1.0f);
    } else {
        projectilePosition = weaponModel->absoluteTransform()[3];
    }

    round.projectile = make_shared<ModelSceneNode>(&_game->sceneGraph(), ammunitionType->model);
    round.projectile->setPosition(projectilePosition);
    round.projectile->detonate();

    shared_ptr<ModelSceneNode> targetModel(target->model());
    glm::vec3 projectileTarget, impactPosition;
    if (targetModel->getNodeAbsolutePosition("impact", impactPosition)) {
        projectileTarget = targetModel->absoluteTransform() * glm::vec4(impactPosition, 1.0f);
    } else {
        projectileTarget = targetModel->absoluteTransform()[3];
    }
    round.projectileTarget = projectileTarget;

    _game->sceneGraph().addRoot(round.projectile);
}

void Combat::updateProjectile(Round &round, float dt) {
    if (!round.projectile) return;

    glm::vec3 projectilePosition(round.projectile->absoluteTransform()[3]);
    glm::vec3 targetToProjectile(round.projectileTarget - projectilePosition);

    float distance = glm::length(targetToProjectile);
    float roundTimeLeft = (round.state == RoundState::FirstTurn ? 0.5f * kRoundDuration : kRoundDuration) - round.time;
    float speed = distance / roundTimeLeft;

    projectilePosition += speed * glm::normalize(targetToProjectile) * dt;

    glm::vec3 dir(glm::normalize(targetToProjectile));
    float facing = glm::half_pi<float>() - glm::atan(dir.x, dir.y);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, projectilePosition);
    transform *= glm::eulerAngleZ(facing);

    round.projectile->setLocalTransform(transform);
}

void Combat::applyAttackResult(const shared_ptr<Creature> &attacker, const shared_ptr<SpatialObject> &target, AttackResult result, int damage) {
    switch (result.type) {
        case AttackResultType::Miss:
        case AttackResultType::AttackResisted:
        case AttackResultType::AttackFailed:
        case AttackResultType::Parried:
        case AttackResultType::Deflected:
            debug(boost::format("Combat: attack missed: '%s' -> '%s'") % attacker->tag() % target->tag(), 2);
            break;
        case AttackResultType::HitSuccessful:
        case AttackResultType::CriticalHit:
        case AttackResultType::AutomaticHit: {
            debug(boost::format("Combat: attack hit: '%s' -> '%s'") % attacker->tag() % target->tag(), 2);
            if (damage == -1) {
                auto effects = _damageResolver.getDamageEffects(attacker);
                for (auto &effect : effects) {
                    target->applyEffect(effect, DurationType::Instant);
                }
            } else {
                target->applyEffect(make_shared<DamageEffect>(damage, DamageType::Universal, attacker), DurationType::Instant);
            }
            break;
        }
        default:
            throw invalid_argument("Combat: applyAttackResult: result is invalid");
    }
}

void Combat::updateActivation() {
    shared_ptr<Creature> partyLeader(_game->party().getLeader());
    bool active = partyLeader && _combatantById.count(partyLeader->id()) != 0;
    if (_active == active) return;

    if (active) {
        onEnterCombatMode();
    } else {
        onExitCombatMode();
    }
    _active = active;
}

void Combat::onEnterCombatMode() {
    _game->module()->area()->setThirdPartyCameraStyle(CameraStyleType::Combat);
}

void Combat::onExitCombatMode() {
    _game->module()->area()->setThirdPartyCameraStyle(CameraStyleType::Default);
}

void Combat::cutsceneAttack(
    const shared_ptr<Creature> &attacker,
    const shared_ptr<SpatialObject> &target,
    int animation,
    AttackResultType result,
    int damage) {

    shared_ptr<Combatant> combatant;

    // If attacker is not a combatant, add him to the combatant map
    auto maybeCombatant = _combatantById.find(attacker->id());
    if (maybeCombatant != _combatantById.end()) {
        combatant = maybeCombatant->second;
    } else {
        combatant = addCombatant(attacker, vector<shared_ptr<Creature>>());
    }

    // If attacker is already participating in a combat round, finish said round
    auto maybeRound = _roundByAttackerId.find(attacker->id());
    if (maybeRound != _roundByAttackerId.end()) {
        finishRound(*maybeRound->second);
    }

    // Add a new combat round
    auto round = make_shared<Round>();
    round->attacker = move(combatant);
    round->target = target;
    round->cutscene = true;
    round->cutsceneAnimation = animation;
    round->cutsceneAttackResult = result;
    round->cutsceneDamage = damage;
    addRound(round);
}

} // namespace game

} // namespace reone
