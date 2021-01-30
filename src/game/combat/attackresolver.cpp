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

#include "attackresolver.h"

#include "../../common/random.h"

#include "../object/creature.h"

#include "attackutil.h"

using namespace std;

namespace reone {

namespace game {

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

AttackResult AttackResolver::getAttackResult(const shared_ptr<Creature> &attacker, const shared_ptr<SpatialObject> &target, bool duel, AttackResultType fixedType) {
    AttackResult result;

    if (fixedType != AttackResultType::Invalid) {
        result.type = fixedType;
    } else {
        int attack = random(1, 20);
        int defense = 10; // TODO: add armor bonus and dexterity modifier

        if (attack == 20) {
            result.type = AttackResultType::AutomaticHit;
        } else if (attack >= defense) {
            result.type = AttackResultType::HitSuccessful;
        } else {
            result.type = AttackResultType::Miss;
        }
    }

    CreatureWieldType attackerWield = attacker->getWieldType();
    CreatureWieldType targetWield = CreatureWieldType::None;

    auto targetCreature = dynamic_pointer_cast<Creature>(target);
    if (targetCreature) {
        targetWield = targetCreature->getWieldType();
    }

    if (duel) {
        if (isMeleeWieldType(attackerWield) && isMeleeWieldType(targetWield)) {
            result.attackerAnimation = CombatAnimation::CinematicMeleeAttack;
            result.animationVariant = random(1, 5);
            result.targetAnimation = isAttackSuccessful(result.type) ? CombatAnimation::CinematicMeleeDamage : CombatAnimation::CinematicMeleeParry;
        } else if (isMeleeWieldType(attackerWield)) {
            result.attackerAnimation = CombatAnimation::MeleeAttack;
            result.animationVariant = random(1, 2);
            result.targetAnimation = isAttackSuccessful(result.type) ? CombatAnimation::MeleeDamage : CombatAnimation::MeleeDodge;
        } else if (isRangedWieldType(attackerWield)) {
            result.attackerAnimation = CombatAnimation::BlasterAttack;
            result.targetAnimation = isAttackSuccessful(result.type) ? CombatAnimation::Damage : CombatAnimation::Dodge;
        } else {
            result.attackerAnimation = CombatAnimation::Attack;
            result.animationVariant = random(1, 2);
            result.targetAnimation = isAttackSuccessful(result.type) ? CombatAnimation::Damage : CombatAnimation::Dodge;
        }
    } else {
        if (isRangedWieldType(attackerWield)) {
            result.attackerAnimation = CombatAnimation::BlasterAttack;
        } else {
            result.attackerAnimation = CombatAnimation::Attack;
            result.animationVariant = random(1, 2);
        }
    }

    result.attackerWieldType = attackerWield;

    return move(result);
}

} // namespace game

} // namespace reone
