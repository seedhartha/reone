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
 *  Implementation of effect-related routines.
 */

#include "declarations.h"

#include "../../../../script/exception/argument.h"
#include "../../../../script/exception/notimpl.h"

#include "../../../core/effect/effect.h"
#include "../../../core/effect/factory.h"
#include "../../../core/game.h"
#include "../../../core/script/routine/argutil.h"
#include "../../../core/script/routine/context.h"
#include "../../../core/types.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable effectAssuredHit(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newAssuredHit();
    return Variable::ofEffect(move(effect));
}

Variable effectHeal(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newHeal();
    return Variable::ofEffect(move(effect));
}

Variable effectDamage(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDamage(0, DamageType::Universal, nullptr);
    return Variable::ofEffect(move(effect));
}

Variable effectAbilityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newAbilityIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageResistance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDamageResistance();
    return Variable::ofEffect(move(effect));
}

Variable effectResurrection(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newResurrection();
    return Variable::ofEffect(move(effect));
}

Variable getIsEffectValid(const vector<Variable> &args, const RoutineContext &ctx) {
    try {
        auto effect = getEffect(args, 0);
        return Variable::ofInt(1);
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable getEffectDurationType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getEffectSubType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getEffectCreator(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable magicalEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable supernaturalEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable extraordinaryEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectACIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newACIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newSavingThrowIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectAttackIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newAttackIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageReduction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDamageReduction();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDamageIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectEntangle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newEntangle();
    return Variable::ofEffect(move(effect));
}

Variable effectDeath(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDeath();
    return Variable::ofEffect(std::move(effect));
}

Variable effectKnockdown(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newKnockdown();
    return Variable::ofEffect(move(effect));
}

Variable effectParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectSpellImmunity(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newSpellImmunity();
    return Variable::ofEffect(move(effect));
}

Variable effectForceJump(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForceJump();
    return Variable::ofEffect(move(effect));
}

Variable effectSleep(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newSleep();
    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newTemporaryForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectConfused(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newConfused();
    return Variable::ofEffect(move(effect));
}

Variable effectFrightened(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newFrightened();
    return Variable::ofEffect(move(effect));
}

Variable effectChoke(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newChoke();
    return Variable::ofEffect(move(effect));
}

Variable effectStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newStunned();
    return Variable::ofEffect(move(effect));
}

Variable effectRegenerate(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newRegenerate();
    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newMovementSpeedIncrease();
    return Variable::ofEffect(move(effect));
}

Variable getEffectType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectAreaOfEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newAreaOfEffect();
    return Variable::ofEffect(move(effect));
}

Variable effectVisualEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newVisual();
    return Variable::ofEffect(move(effect));
}

Variable effectLinkEffects(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newLinkEffects();
    return Variable::ofEffect(move(effect));
}

Variable effectBeam(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newBeam();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResistanceIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForceResistanceIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectBodyFuel(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newBodyFuel();
    return Variable::ofEffect(move(effect));
}

Variable effectPoison(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newPoison();
    return Variable::ofEffect(move(effect));
}

Variable effectAssuredDeflection(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newAssuredDeflection();
    return Variable::ofEffect(move(effect));
}

Variable effectForcePushTargeted(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForcePushTargeted();
    return Variable::ofEffect(move(effect));
}

Variable effectHaste(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newHaste();
    return Variable::ofEffect(move(effect));
}

Variable effectImmunity(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newImmunity();
    return Variable::ofEffect(move(effect));
}

Variable getEffectSpellId(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectDamageImmunityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDamageImmunityIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryHitpoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newTemporaryHitPoints();
    return Variable::ofEffect(move(effect));
}

Variable effectSkillIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newSkillIncrease();
    return Variable::ofEffect(move(effect));
}

Variable versusAlignmentEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable versusRacialTypeEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable versusTrapEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectDamageForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDamageForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectHealForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newHealForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectHitPointChangeWhenDying(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newHitPointsChangeWhenDying();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidStun(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDroidStun();
    return Variable::ofEffect(move(effect));
}

Variable effectForcePushed(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForcePushed();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResisted(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForceResisted();
    return Variable::ofEffect(move(effect));
}

Variable effectForceFizzle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForceFizzle();
    return Variable::ofEffect(move(effect));
}

Variable effectAbilityDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newAbilityDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectAttackDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newAttackDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDamageDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageImmunityDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDamageImmunityDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectACDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newACDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newMovementSpeedDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newSavingThrowDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSkillDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newSkillDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResistanceDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForceResistanceDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectInvisibility(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newInvisibility();
    return Variable::ofEffect(move(effect));
}

Variable effectConcealment(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newConcealment();
    return Variable::ofEffect(move(effect));
}

Variable effectForceShield(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForceShield();
    return Variable::ofEffect(move(effect));
}

Variable effectDispelMagicAll(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDispelMagicAll();
    return Variable::ofEffect(move(effect));
}

Variable effectDisguise(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDisguise();
    return Variable::ofEffect(move(effect));
}

Variable effectTrueSeeing(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newTrueSeeing();
    return Variable::ofEffect(move(effect));
}

Variable effectSeeInvisible(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newSeeInvisible();
    return Variable::ofEffect(move(effect));
}

Variable effectTimeStop(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newTimeStop();
    return Variable::ofEffect(move(effect));
}

Variable effectBlasterDeflectionIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newBlasterDeflectionIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectBlasterDeflectionDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newBlasterDeflectionDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectSpellLevelAbsorption(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newSpellLevelAbsorption();
    return Variable::ofEffect(move(effect));
}

Variable effectDispelMagicBest(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDispelMagicBest();
    return Variable::ofEffect(move(effect));
}

Variable effectMissChance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newMissChance();
    return Variable::ofEffect(move(effect));
}

Variable effectModifyAttacks(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newModifyAttacks();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageShield(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDamageShield();
    return Variable::ofEffect(move(effect));
}

Variable setEffectIcon(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectForceDrain(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForceDrain();
    return Variable::ofEffect(move(effect));
}

Variable effectPsychicStatic(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newPsychicStatic();
    return Variable::ofEffect(move(effect));
}

Variable effectLightsaberThrow(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newLightsaberThrow();
    return Variable::ofEffect(move(effect));
}

Variable effectWhirlWind(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newWhirlWind();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newCutsceneHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newCutsceneParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newCutsceneStunned();
    return Variable::ofEffect(move(effect));
}

} // namespace routine

} // namespace game

} // namespace reone
