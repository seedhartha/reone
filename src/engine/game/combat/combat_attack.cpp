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

#include <stdexcept>

#include "../../common/log.h"
#include "../../common/random.h"

#include "../objectconverter.h"

using namespace std;

namespace reone {

namespace game {

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

AttackResultType Combat::determineAttackResult(const Attack &attack) const {
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
    } else if (roll > 1 && roll + attack.attacker->getAttackBonus() >= defense) { // 1 is automatic miss
        result = AttackResultType::HitSuccessful;
    }

    // Critical threat
    if (isAttackSuccessful(result)) {
        int criticalThreat;
        shared_ptr<Item> rightWeapon(attack.attacker->getEquippedItem(InventorySlot::rightWeapon));
        if (rightWeapon) {
            criticalThreat = rightWeapon->criticalThreat();
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
    auto target = ObjectConverter::toCreature(attack.target);
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

void Combat::applyAttackResult(const Attack &attack) {
    // Determine critical hit multiplier
    int criticalHitMultiplier = 2;
    shared_ptr<Item> rightWeapon(attack.attacker->getEquippedItem(InventorySlot::rightWeapon));
    if (rightWeapon) {
        criticalHitMultiplier = rightWeapon->criticalHitMultiplier();
    }

    switch (attack.resultType) {
        case AttackResultType::Miss:
        case AttackResultType::AttackResisted:
        case AttackResultType::AttackFailed:
        case AttackResultType::Parried:
        case AttackResultType::Deflected:
            debug(boost::format("Combat: attack missed: %s -> %s") % attack.attacker->tag() % attack.target->tag(), 2, DebugChannels::combat);
            break;
        case AttackResultType::HitSuccessful:
        case AttackResultType::AutomaticHit: {
            debug(boost::format("Combat: attack hit: %s -> %s") % attack.attacker->tag() % attack.target->tag(), 2, DebugChannels::combat);
            if (attack.damage == -1) {
                auto effects = getDamageEffects(attack.attacker);
                for (auto &effect : effects) {
                    attack.target->applyEffect(effect, DurationType::Instant);
                }
            } else {
                attack.target->applyEffect(make_shared<DamageEffect>(attack.damage, DamageType::Universal, attack.attacker), DurationType::Instant);
            }
            break;
        }
        case AttackResultType::CriticalHit: {
            debug(boost::format("Combat: attack critical hit: %s -> %s") % attack.attacker->tag() % attack.target->tag(), 2, DebugChannels::combat);
            if (attack.damage == -1) {
                auto effects = getDamageEffects(attack.attacker, criticalHitMultiplier);
                for (auto &effect : effects) {
                    attack.target->applyEffect(effect, DurationType::Instant);
                }
            } else {
                attack.target->applyEffect(make_shared<DamageEffect>(criticalHitMultiplier * attack.damage, DamageType::Universal, attack.attacker), DurationType::Instant);
            }
            break;
        }
        default:
            throw logic_error("Unsupported attack result");
    }
}

} // namespace game

} // namespace reone
