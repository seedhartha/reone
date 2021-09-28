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

#include "../common/logutil.h"
#include "../common/randomutil.h"

#include "effect/factory.h"
#include "game.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kRoundDuration = 3.0f;
static constexpr float kProjectileDelay = 0.5f;
static constexpr float kDeactivateDelay = 8.0f;

static constexpr char kModelEventDetonate[] = "detonate";
static constexpr float kProjectileSpeed = 16.0f;

static unique_ptr<Combat::Attack> makeAttack(shared_ptr<Creature> attacker, shared_ptr<SpatialObject> target, ObjectAction *action, AttackResultType resultType, int damage) {
    auto attack = make_unique<Combat::Attack>();
    attack->attacker = move(attacker);
    attack->target = move(target);
    attack->action = action;
    attack->resultType = resultType;
    attack->damage = damage;
    return move(attack);
}

static bool isRoundPastFirstAttack(float time) {
    return time >= 0.5f * kRoundDuration;
}

void Combat::addAttack(shared_ptr<Creature> attacker, shared_ptr<SpatialObject> target, ObjectAction *action, AttackResultType resultType, int damage) {
    RoundMap::iterator maybeRound;

    // If attacker has already started a combat round, do nothing
    maybeRound = _roundByAttacker.find(attacker->id());
    if (maybeRound != _roundByAttacker.end())
        return;

    // If there is an incomplete combat round where attacker and target roles are reversed, append to that round
    maybeRound = _roundByAttacker.find(target->id());
    if (maybeRound != _roundByAttacker.end()) {
        Round &round = *maybeRound->second;
        if (round.attack1->target == attacker) {
            if (!round.attack2 && !isRoundPastFirstAttack(round.time)) {
                round.attack2 = makeAttack(attacker, target, action, resultType, damage);
                round.duel = true;
                debug(boost::format("Append attack: %s -> %s") % attacker->tag() % target->tag(), LogChannels::combat);
            }
            return;
        }
    }

    // Otherwise, start a new combat round
    auto round = make_unique<Round>();
    round->attack1 = makeAttack(attacker, target, action, resultType, damage);
    _roundByAttacker.insert(make_pair(attacker->id(), move(round)));
    debug(boost::format("Start round: %s -> %s") % attacker->tag() % target->tag(), LogChannels::combat);
}

void Combat::update(float dt) {
    for (auto it = _roundByAttacker.begin(); it != _roundByAttacker.end();) {
        Round &round = *it->second;
        updateRound(round, dt);

        // Remove finished combat rounds
        if (round.state == RoundState::Finished) {
            it = _roundByAttacker.erase(it);
        } else {
            ++it;
        }
    }
}

void Combat::updateRound(Round &round, float dt) {
    round.time = glm::min(round.time + dt, kRoundDuration);

    switch (round.state) {
    case RoundState::Started: {
        if (round.attack1->resultType == AttackResultType::Invalid) {
            round.attack1->resultType = determineAttackResult(*round.attack1);
        }
        startAttack(*round.attack1, round.duel);
        round.state = RoundState::FirstAttack;
        break;
    }
    case RoundState::FirstAttack: {
        if (isRoundPastFirstAttack(round.time)) {
            resetProjectile(round);
            applyAttackResult(*round.attack1);

            // Off-hand attack
            if (round.attack1->attacker->isTwoWeaponFighting()) {
                round.attack1->resultType = determineAttackResult(*round.attack1, true);
                applyAttackResult(*round.attack1, true);
            }
            // TODO: additional attacks from Feats and Force Powers

            if (round.attack2) {
                if (round.attack2->resultType == AttackResultType::Invalid) {
                    round.attack2->resultType = determineAttackResult(*round.attack2);
                }
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

                // Off-hand attack
                if (round.attack2->attacker->isTwoWeaponFighting()) {
                    round.attack2->resultType = determineAttackResult(*round.attack2, true);
                    applyAttackResult(*round.attack2, true);
                }
                // TODO: additional attacks from Feats and Force Powers
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
    AttackAnimation animation = determineAttackAnimation(attack, duel);

    attack.attacker->face(*attack.target);
    attack.attacker->setMovementType(Creature::MovementType::None);
    attack.attacker->setMovementRestricted(true);
    attack.attacker->activateCombat();
    attack.attacker->setAttackTarget(attack.target);
    attack.attacker->playAnimation(animation.attackerAnimation, animation.attackerWieldType, animation.animationVariant);

    if (duel) {
        auto target = static_pointer_cast<Creature>(attack.target);
        target->face(*attack.attacker);
        target->setMovementType(Creature::MovementType::None);
        target->setMovementRestricted(true);
        target->activateCombat();
        target->setAttackTarget(attack.attacker);
        target->playAnimation(animation.targetAnimation, animation.attackerWieldType, animation.animationVariant);
    }
}

static void finishAttack(Combat::Attack &attack) {
    if (attack.action) {
        attack.action->complete();
    }
    attack.attacker->deactivateCombat(kDeactivateDelay);
    attack.attacker->setAttackTarget(nullptr);
    attack.attacker->setMovementRestricted(false);
    attack.attacker->runEndRoundScript();
}

void Combat::finishRound(Round &round) {
    finishAttack(*round.attack1);
    if (round.attack2) {
        finishAttack(*round.attack2);
    }
    round.state = RoundState::Finished;
    debug(boost::format("Finish round: %s -> %s") % round.attack1->attacker->tag() % round.attack1->target->tag(), LogChannels::combat);
}

static bool isAttackSuccessful(AttackResultType result) {
    switch (result) {
    case AttackResultType::HitSuccessful:
    case AttackResultType::CriticalHit:
    case AttackResultType::AutomaticHit:
        return true;
    default:
        return false;
    }
}

AttackResultType Combat::determineAttackResult(const Attack &attack, bool offHand) const {
    auto result = AttackResultType::Miss;

    // Determine defense of a target
    int defense;
    if (attack.target->type() == ObjectType::Creature) {
        defense = static_pointer_cast<Creature>(attack.target)->getDefense();
    } else {
        defense = 10;
    }

    // Attack roll
    int roll = random(1, 20);
    if (roll == 20) {
        result = AttackResultType::AutomaticHit;
    } else if (roll > 1 && roll + attack.attacker->getAttackBonus(offHand) >= defense) { // 1 is automatic miss
        result = AttackResultType::HitSuccessful;
    }

    // Critical threat
    if (isAttackSuccessful(result)) {
        int criticalThreat;
        shared_ptr<Item> weapon(attack.attacker->getEquippedItem(offHand ? InventorySlot::leftWeapon : InventorySlot::rightWeapon));
        if (weapon) {
            criticalThreat = weapon->criticalThreat();
        } else {
            criticalThreat = 1;
        }
        if (roll > 20 - criticalThreat) {
            // Critical hit roll
            int criticalRoll = random(1, 20);
            if (criticalRoll + attack.attacker->getAttackBonus() >= defense) {
                result = AttackResultType::CriticalHit;
            }
        }
    }

    return result;
}

static bool isMeleeWieldType(CreatureWieldType type) {
    switch (type) {
    case CreatureWieldType::SingleSword:
    case CreatureWieldType::DoubleBladedSword:
    case CreatureWieldType::DualSwords:
        return true;
    default:
        return false;
    }
}

static bool isRangedWieldType(CreatureWieldType type) {
    switch (type) {
    case CreatureWieldType::BlasterPistol:
    case CreatureWieldType::DualPistols:
    case CreatureWieldType::BlasterRifle:
    case CreatureWieldType::HeavyWeapon:
        return true;
    default:
        return false;
    }
}

Combat::AttackAnimation Combat::determineAttackAnimation(const Attack &attack, bool duel) const {
    AttackAnimation result;

    // Determine wield types

    result.attackerWieldType = attack.attacker->getWieldType();

    auto targetWield = CreatureWieldType::None;
    auto target = dynamic_pointer_cast<Creature>(attack.target);
    if (target) {
        targetWield = target->getWieldType();
    }

    // Determine animation

    if (duel) {
        if (isMeleeWieldType(result.attackerWieldType) && isMeleeWieldType(targetWield)) {
            result.attackerAnimation = CombatAnimation::CinematicMeleeAttack;
            result.animationVariant = random(1, 5);
            result.targetAnimation = isAttackSuccessful(attack.resultType) ? CombatAnimation::CinematicMeleeDamage : CombatAnimation::CinematicMeleeParry;
        } else if (isMeleeWieldType(result.attackerWieldType)) {
            result.attackerAnimation = CombatAnimation::MeleeAttack;
            result.animationVariant = random(1, 2);
            result.targetAnimation = isAttackSuccessful(attack.resultType) ? CombatAnimation::MeleeDamage : CombatAnimation::MeleeDodge;
        } else if (isRangedWieldType(result.attackerWieldType)) {
            result.attackerAnimation = CombatAnimation::BlasterAttack;
            result.targetAnimation = isAttackSuccessful(attack.resultType) ? CombatAnimation::Damage : CombatAnimation::Dodge;
        } else {
            result.attackerAnimation = CombatAnimation::Attack;
            result.animationVariant = random(1, 2);
            result.targetAnimation = isAttackSuccessful(attack.resultType) ? CombatAnimation::Damage : CombatAnimation::Dodge;
        }
    } else {
        if (isRangedWieldType(result.attackerWieldType)) {
            result.attackerAnimation = CombatAnimation::BlasterAttack;
        } else {
            result.attackerAnimation = CombatAnimation::Attack;
            result.animationVariant = random(1, 2);
        }
    }

    return move(result);
}

void Combat::applyAttackResult(const Attack &attack, bool offHand) {
    // Determine critical hit multiplier
    int criticalHitMultiplier = 2;
    shared_ptr<Item> weapon(attack.attacker->getEquippedItem(offHand ? InventorySlot::leftWeapon : InventorySlot::rightWeapon));
    if (weapon) {
        criticalHitMultiplier = weapon->criticalHitMultiplier();
    }

    switch (attack.resultType) {
    case AttackResultType::Miss:
    case AttackResultType::AttackResisted:
    case AttackResultType::AttackFailed:
    case AttackResultType::Parried:
    case AttackResultType::Deflected:
        debug(boost::format("Attack missed: %s -> %s") % attack.attacker->tag() % attack.target->tag(), LogChannels::combat);
        break;
    case AttackResultType::HitSuccessful:
    case AttackResultType::AutomaticHit: {
        debug(boost::format("Attack hit: %s -> %s") % attack.attacker->tag() % attack.target->tag(), LogChannels::combat);
        if (attack.damage == -1) {
            auto effects = getDamageEffects(attack.attacker, offHand);
            for (auto &effect : effects) {
                attack.target->applyEffect(effect, DurationType::Instant);
            }
        } else {
            shared_ptr<DamageEffect> effect(_effectFactory.newDamage(attack.damage, DamageType::Universal, attack.attacker));
            attack.target->applyEffect(move(effect), DurationType::Instant);
        }
        break;
    }
    case AttackResultType::CriticalHit: {
        debug(boost::format("Attack critical hit: %s -> %s") % attack.attacker->tag() % attack.target->tag(), LogChannels::combat);
        if (attack.damage == -1) {
            auto effects = getDamageEffects(attack.attacker, offHand, criticalHitMultiplier);
            for (auto &effect : effects) {
                attack.target->applyEffect(effect, DurationType::Instant);
            }
        } else {
            shared_ptr<DamageEffect> effect(_effectFactory.newDamage(criticalHitMultiplier * attack.damage, DamageType::Universal, attack.attacker));
            attack.target->applyEffect(move(effect), DurationType::Instant);
        }
        break;
    }
    default:
        throw logic_error("Unsupported attack result");
    }
}

vector<shared_ptr<DamageEffect>> Combat::getDamageEffects(shared_ptr<Creature> damager, bool offHand, int multiplier) const {
    shared_ptr<Item> weapon(damager->getEquippedItem(offHand ? InventorySlot::leftWeapon : InventorySlot::rightWeapon));
    int amount = 0;
    auto type = DamageType::Bludgeoning;

    if (weapon) {
        for (int i = 0; i < weapon->numDice(); ++i) {
            amount += random(1, weapon->dieToRoll());
        }
        type = static_cast<DamageType>(weapon->damageFlags());
    }
    amount = glm::max(1, amount);
    shared_ptr<DamageEffect> effect(_effectFactory.newDamage(multiplier * amount, type, move(damager)));

    return vector<shared_ptr<DamageEffect>> {move(effect)};
}

void Combat::fireProjectile(const shared_ptr<Creature> &attacker, const shared_ptr<SpatialObject> &target, Round &round) {
    auto attackerModel = static_pointer_cast<ModelSceneNode>(attacker->sceneNode());
    auto targetModel = static_pointer_cast<ModelSceneNode>(target->sceneNode());
    if (!attackerModel || !targetModel)
        return;

    shared_ptr<Item> weapon(attacker->getEquippedItem(InventorySlot::rightWeapon));
    if (!weapon)
        return;

    shared_ptr<Item::AmmunitionType> ammunitionType(weapon->ammunitionType());
    if (!ammunitionType)
        return;

    auto weaponModel = static_pointer_cast<ModelSceneNode>(attackerModel->getAttachment("rhand"));
    if (!weaponModel)
        return;

    // Determine projectile position
    glm::vec3 projectilePos;
    shared_ptr<ModelNode> bulletHook(weaponModel->model()->getNodeByName("bullethook"));
    if (bulletHook) {
        projectilePos = weaponModel->absoluteTransform() * bulletHook->absoluteTransform()[3];
    } else {
        projectilePos = weaponModel->getOrigin();
    }

    // Determine projectile direction
    glm::vec3 projectileTarget;
    shared_ptr<ModelNode> impact(targetModel->model()->getNodeByName("impact"));
    if (impact) {
        projectileTarget = targetModel->absoluteTransform() * impact->absoluteTransform()[3];
    } else {
        projectileTarget = targetModel->getOrigin();
    }
    round.projectileDir = glm::normalize(projectileTarget - projectilePos);

    // Create and add a projectile to the scene graph
    round.projectile = _sceneGraph.newModel(ammunitionType->model, ModelUsage::Projectile);
    round.projectile->signalEvent(kModelEventDetonate);
    round.projectile->setLocalTransform(glm::translate(projectilePos));
    _sceneGraph.addRoot(round.projectile);

    // Play shot sound, if any
    weapon->playShotSound(0, projectilePos);
}

void Combat::updateProjectile(Round &round, float dt) {
    glm::vec3 position(round.projectile->absoluteTransform()[3]);
    position += kProjectileSpeed * round.projectileDir * dt;

    float facing = glm::half_pi<float>() - glm::atan(round.projectileDir.x, round.projectileDir.y);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, position);
    transform *= glm::eulerAngleZ(facing);

    round.projectile->setLocalTransform(transform);
}

void Combat::resetProjectile(Round &round) {
    if (round.projectile) {
        _sceneGraph.removeRoot(round.projectile);
        round.projectile.reset();
    }
}

} // namespace game

} // namespace reone
