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

/** @file
 *  Implementation of effect factory routines.
 */

#include "../declarations.h"

#include "../../../../game/effect.h"
#include "../../../../game/effect/factory.h"
#include "../../../../game/game.h"
#include "../../../../game/services.h"
#include "../../../../game/types.h"
#include "../../../../script/exception/notimpl.h"

#include "../argutil.h"
#include "../context.h"

using namespace std;

using namespace reone::game;
using namespace reone::script;

namespace reone {

namespace kotor {

namespace routine {

static constexpr int kAcVsDamageTypeAll = 8199;

Variable effectAssuredHit(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newAssuredHit();
    return Variable::ofEffect(move(effect));
}

Variable effectHeal(const vector<Variable> &args, const RoutineContext &ctx) {
    int damageToHeal = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newHeal(damageToHeal);

    return Variable::ofEffect(move(effect));
}

Variable effectDamage(const vector<Variable> &args, const RoutineContext &ctx) {
    int damageAmount = getInt(args, 0);
    auto damageType = getIntAsEnumOrElse(args, 1, DamageType::Universal);
    auto damagePower = getIntAsEnumOrElse(args, 2, DamagePower::Normal);

    auto effect = ctx.game.effectFactory().newDamage(damageAmount, damageType, damagePower, nullptr);

    return Variable::ofEffect(move(effect));
}

Variable effectAbilityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto abilityToIncrease = getIntAsEnum<Ability>(args, 0);
    int modifyBy = getInt(args, 1);

    auto effect = ctx.game.effectFactory().newAbilityIncrease(abilityToIncrease, modifyBy);

    return Variable::ofEffect(move(effect));
}

Variable effectDamageResistance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto damageType = getIntAsEnum<DamageType>(args, 0);
    int amount = getInt(args, 1);
    int limit = getIntOrElse(args, 2, 0);

    auto effect = ctx.game.effectFactory().newDamageResistance(damageType, amount, limit);

    return Variable::ofEffect(move(effect));
}

Variable effectResurrection(const vector<Variable> &args, const RoutineContext &ctx) {
    int hpPercent = getIntOrElse(args, 0, 0);
    auto effect = ctx.game.effectFactory().newResurrection(hpPercent);

    return Variable::ofEffect(move(effect));
}

Variable effectACIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);
    auto modifyType = getIntAsEnumOrElse(args, 1, ACBonus::Dodge);
    int damageType = getIntOrElse(args, 2, kAcVsDamageTypeAll);

    auto effect = ctx.game.effectFactory().newACIncrease(value, modifyType, damageType);

    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int save = getInt(args, 0);
    int value = getInt(args, 1);
    auto saveType = getIntAsEnumOrElse(args, 2, SavingThrowType::All);

    auto effect = ctx.game.effectFactory().newSavingThrowIncrease(save, value, saveType);

    return Variable::ofEffect(move(effect));
}

Variable effectAttackIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int bonus = getInt(args, 0);
    auto modifierType = getIntAsEnumOrElse(args, 1, AttackBonus::Misc);

    auto effect = ctx.game.effectFactory().newAttackIncrease(bonus, modifierType);

    return Variable::ofEffect(move(effect));
}

Variable effectDamageIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDamageIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectEntangle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newEntangle();
    return Variable::ofEffect(move(effect));
}

Variable effectDeath(const vector<Variable> &args, const RoutineContext &ctx) {
    bool spectacularDeath = getIntAsBoolOrElse(args, 0, false);
    bool displayFeedback = getIntAsBoolOrElse(args, 1, true);
    bool noFadeAway = getIntAsBoolOrElse(args, 2, false);

    auto effect = ctx.game.effectFactory().newDeath(spectacularDeath, displayFeedback, noFadeAway);

    return Variable::ofEffect(std::move(effect));
}

Variable effectParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectSleep(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newSleep();
    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    int tempForce = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newTemporaryForcePoints(tempForce);

    return Variable::ofEffect(move(effect));
}

Variable effectChoke(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newChoke();
    return Variable::ofEffect(move(effect));
}

Variable effectStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newStunned();
    return Variable::ofEffect(move(effect));
}

Variable effectRegenerate(const vector<Variable> &args, const RoutineContext &ctx) {
    int amount = getInt(args, 0);
    float intervalSeconds = getFloat(args, 1);

    auto effect = ctx.game.effectFactory().newRegenerate(amount, intervalSeconds);

    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int newSpeedPercent = getInt(args, 0);

    auto effect = ctx.game.effectFactory().newMovementSpeedIncrease(newSpeedPercent);

    return Variable::ofEffect(move(effect));
}

Variable effectVisualEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    int visualEffectId = getInt(args, 0);
    bool missEffect = getIntAsBoolOrElse(args, 1, false);

    auto effect = ctx.game.effectFactory().newVisual(visualEffectId, missEffect);

    return Variable::ofEffect(move(effect));
}

Variable effectLinkEffects(const vector<Variable> &args, const RoutineContext &ctx) {
    auto childEffect = getEffect(args, 0);
    auto parentEffect = getEffect(args, 1);

    auto effect = ctx.game.effectFactory().newLinkEffects(move(childEffect), move(parentEffect));

    return Variable::ofEffect(move(effect));
}

Variable effectBeam(const vector<Variable> &args, const RoutineContext &ctx) {
    int beamVisualEffect = getInt(args, 0);
    auto effector = getObject(args, 1, ctx);
    auto bodyPart = getIntAsEnum<BodyNode>(args, 2);
    bool missEffect = getIntAsBoolOrElse(args, 3, false);

    auto effect = ctx.game.effectFactory().newBeam(beamVisualEffect, std::move(effector), bodyPart, missEffect);

    return Variable::ofEffect(move(effect));
}

Variable effectForceResistanceIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newForceResistanceIncrease(value);

    return Variable::ofEffect(move(effect));
}

Variable effectBodyFuel(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newBodyFuel();
    return Variable::ofEffect(move(effect));
}

Variable effectPoison(const vector<Variable> &args, const RoutineContext &ctx) {
    auto poisonType = getIntAsEnum<Poison>(args, 0);
    auto effect = ctx.game.effectFactory().newPoison(poisonType);

    return Variable::ofEffect(move(effect));
}

Variable effectForcePushTargeted(const vector<Variable> &args, const RoutineContext &ctx) {
    auto centre = getLocationArgument(args, 0);
    bool ignoreTestDirectLine = getIntAsBoolOrElse(args, 1, false);

    auto effect = ctx.game.effectFactory().newForcePushTargeted(centre, ignoreTestDirectLine);

    return Variable::ofEffect(move(effect));
}

Variable effectImmunity(const vector<Variable> &args, const RoutineContext &ctx) {
    auto immunityType = getIntAsEnum<ImmunityType>(args, 0);
    auto effect = ctx.game.effectFactory().newImmunity(immunityType);

    return Variable::ofEffect(move(effect));
}

Variable effectDamageImmunityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto damageType = getIntAsEnum<DamageType>(args, 0);
    int percentImmunity = getInt(args, 1);

    auto effect = ctx.game.effectFactory().newDamageImmunityIncrease(damageType, percentImmunity);

    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryHitpoints(const vector<Variable> &args, const RoutineContext &ctx) {
    int hitPoints = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newTemporaryHitPoints(hitPoints);

    return Variable::ofEffect(move(effect));
}

Variable effectSkillIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto skill = getIntAsEnum<SkillType>(args, 0);
    int value = getInt(args, 1);

    auto effect = ctx.game.effectFactory().newSkillIncrease(skill, value);

    return Variable::ofEffect(move(effect));
}

Variable effectDamageForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    int damage = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newDamageForcePoints(damage);

    return Variable::ofEffect(move(effect));
}

Variable effectHealForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    int heal = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newHealForcePoints(heal);

    return Variable::ofEffect(move(effect));
}

Variable effectDroidStun(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDroidStun();
    return Variable::ofEffect(move(effect));
}

Variable effectForcePushed(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForcePushed();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResisted(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForceResisted();
    return Variable::ofEffect(move(effect));
}

Variable effectForceFizzle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForceFizzle();
    return Variable::ofEffect(move(effect));
}

Variable effectAbilityDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto ability = getIntAsEnum<Ability>(args, 0);
    int modifyBy = getInt(args, 1);

    auto effect = ctx.game.effectFactory().newAbilityDecrease(ability, modifyBy);

    return Variable::ofEffect(move(effect));
}

Variable effectAttackDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newAttackDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int penalty = getInt(args, 0);
    auto modifierType = getIntAsEnumOrElse(args, 1, AttackBonus::Misc);

    auto effect = ctx.game.effectFactory().newDamageDecrease(penalty, modifierType);

    return Variable::ofEffect(move(effect));
}

Variable effectACDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);
    auto modifyType = getIntAsEnumOrElse(args, 1, ACBonus::Dodge);
    int damageType = getIntOrElse(args, 2, kAcVsDamageTypeAll);

    auto effect = ctx.game.effectFactory().newACDecrease(value, modifyType, damageType);

    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int percentChange = getInt(args, 0);

    auto effect = ctx.game.effectFactory().newMovementSpeedDecrease(percentChange);

    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int save = getInt(args, 0);
    int value = getInt(args, 1);
    auto saveType = getIntAsEnumOrElse(args, 2, SavingThrowType::All);

    auto effect = ctx.game.effectFactory().newSavingThrowDecrease(save, value, saveType);

    return Variable::ofEffect(move(effect));
}

Variable effectInvisibility(const vector<Variable> &args, const RoutineContext &ctx) {
    auto invisibilityType = getIntAsEnum<InvisibilityType>(args, 0);
    auto effect = ctx.game.effectFactory().newInvisibility(invisibilityType);

    return Variable::ofEffect(move(effect));
}

Variable effectForceShield(const vector<Variable> &args, const RoutineContext &ctx) {
    int shield = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newForceShield(shield);

    return Variable::ofEffect(move(effect));
}

Variable effectDisguise(const vector<Variable> &args, const RoutineContext &ctx) {
    int disguiseAppearance = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newDisguise(disguiseAppearance);

    return Variable::ofEffect(move(effect));
}

Variable effectTrueSeeing(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newTrueSeeing();
    return Variable::ofEffect(move(effect));
}

Variable effectBlasterDeflectionIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int change = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newBlasterDeflectionIncrease(change);

    return Variable::ofEffect(move(effect));
}

Variable effectHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectModifyAttacks(const vector<Variable> &args, const RoutineContext &ctx) {
    int attacks = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newModifyAttacks(attacks);

    return Variable::ofEffect(move(effect));
}

Variable effectPsychicStatic(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newPsychicStatic();
    return Variable::ofEffect(move(effect));
}

Variable effectLightsaberThrow(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target1 = getObject(args, 0, ctx);
    auto target2 = getObjectOrNull(args, 1, ctx);
    auto target3 = getObjectOrNull(args, 2, ctx);
    int advancedDamage = getInt(args, 3);

    auto effect = ctx.game.effectFactory().newLightsaberThrow(move(target1), move(target2), move(target3), advancedDamage);

    return Variable::ofEffect(move(effect));
}

Variable effectWhirlWind(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newWhirlWind();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newCutsceneHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newCutsceneParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newCutsceneStunned();
    return Variable::ofEffect(move(effect));
}

// TSL

Variable effectForceBody(const vector<Variable> &args, const RoutineContext &ctx) {
    int level = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newForceBody(level);

    return Variable::ofEffect(move(effect));
}

Variable effectFury(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newFury();
    return Variable::ofEffect(move(effect));
}

Variable effectBlind(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newBlind();
    return Variable::ofEffect(move(effect));
}

Variable effectVPRegenModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    int percent = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newVPRegenModifier(percent);

    return Variable::ofEffect(move(effect));
}

Variable effectCrush(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newCrush();
    return Variable::ofEffect(move(effect));
}

Variable effectForceSight(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForceSight();
    return Variable::ofEffect(move(effect));
}

Variable effectMindTrick(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newMindTrick();
    return Variable::ofEffect(move(effect));
}

Variable effectFactionModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    int newFaction = getInt(args, 0);
    auto effect = ctx.game.effectFactory().newFactionModifier(newFaction);

    return Variable::ofEffect(move(effect));
}

Variable effectDroidScramble(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDroidScramble();
    return Variable::ofEffect(move(effect));
}

// END TSL

} // namespace routine

} // namespace kotor

} // namespace reone
