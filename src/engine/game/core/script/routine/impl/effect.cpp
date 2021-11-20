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

#include "../declarations.h"

#include "../../../../../script/exception/argument.h"
#include "../../../../../script/exception/notimpl.h"

#include "../../../effect/effect.h"
#include "../../../effect/factory.h"
#include "../../../game.h"
#include "../../../script/routine/argutil.h"
#include "../../../script/routine/context.h"
#include "../../../services.h"
#include "../../../types.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable effectAssuredHit(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newAssuredHit();
    return Variable::ofEffect(move(effect));
}

Variable effectHeal(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newHeal();
    return Variable::ofEffect(move(effect));
}

Variable effectDamage(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDamage(0, DamageType::Universal, nullptr);
    return Variable::ofEffect(move(effect));
}

Variable effectAbilityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newAbilityIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageResistance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDamageResistance();
    return Variable::ofEffect(move(effect));
}

Variable effectResurrection(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newResurrection();
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
    auto effect = ctx.services.effectFactory.newACIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newSavingThrowIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectAttackIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newAttackIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageReduction(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDamageReduction();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDamageIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectEntangle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newEntangle();
    return Variable::ofEffect(move(effect));
}

Variable effectDeath(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDeath();
    return Variable::ofEffect(std::move(effect));
}

Variable effectKnockdown(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newKnockdown();
    return Variable::ofEffect(move(effect));
}

Variable effectParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectSpellImmunity(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newSpellImmunity();
    return Variable::ofEffect(move(effect));
}

Variable effectForceJump(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForceJump();
    return Variable::ofEffect(move(effect));
}

Variable effectSleep(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newSleep();
    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newTemporaryForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectConfused(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newConfused();
    return Variable::ofEffect(move(effect));
}

Variable effectFrightened(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newFrightened();
    return Variable::ofEffect(move(effect));
}

Variable effectChoke(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newChoke();
    return Variable::ofEffect(move(effect));
}

Variable effectStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newStunned();
    return Variable::ofEffect(move(effect));
}

Variable effectRegenerate(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newRegenerate();
    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newMovementSpeedIncrease();
    return Variable::ofEffect(move(effect));
}

Variable getEffectType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectAreaOfEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newAreaOfEffect();
    return Variable::ofEffect(move(effect));
}

Variable effectVisualEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newVisual();
    return Variable::ofEffect(move(effect));
}

Variable effectLinkEffects(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newLinkEffects();
    return Variable::ofEffect(move(effect));
}

Variable effectBeam(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newBeam();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResistanceIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForceResistanceIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectBodyFuel(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newBodyFuel();
    return Variable::ofEffect(move(effect));
}

Variable effectPoison(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newPoison();
    return Variable::ofEffect(move(effect));
}

Variable effectAssuredDeflection(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newAssuredDeflection();
    return Variable::ofEffect(move(effect));
}

Variable effectForcePushTargeted(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForcePushTargeted();
    return Variable::ofEffect(move(effect));
}

Variable effectHaste(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newHaste();
    return Variable::ofEffect(move(effect));
}

Variable effectImmunity(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newImmunity();
    return Variable::ofEffect(move(effect));
}

Variable getEffectSpellId(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectDamageImmunityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDamageImmunityIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryHitpoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newTemporaryHitPoints();
    return Variable::ofEffect(move(effect));
}

Variable effectSkillIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newSkillIncrease();
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
    auto effect = ctx.services.effectFactory.newDamageForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectHealForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newHealForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectHitPointChangeWhenDying(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newHitPointsChangeWhenDying();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidStun(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDroidStun();
    return Variable::ofEffect(move(effect));
}

Variable effectForcePushed(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForcePushed();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResisted(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForceResisted();
    return Variable::ofEffect(move(effect));
}

Variable effectForceFizzle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForceFizzle();
    return Variable::ofEffect(move(effect));
}

Variable effectAbilityDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newAbilityDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectAttackDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newAttackDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDamageDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageImmunityDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDamageImmunityDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectACDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newACDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newMovementSpeedDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newSavingThrowDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSkillDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newSkillDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResistanceDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForceResistanceDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectInvisibility(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newInvisibility();
    return Variable::ofEffect(move(effect));
}

Variable effectConcealment(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newConcealment();
    return Variable::ofEffect(move(effect));
}

Variable effectForceShield(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForceShield();
    return Variable::ofEffect(move(effect));
}

Variable effectDispelMagicAll(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDispelMagicAll();
    return Variable::ofEffect(move(effect));
}

Variable effectDisguise(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDisguise();
    return Variable::ofEffect(move(effect));
}

Variable effectTrueSeeing(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newTrueSeeing();
    return Variable::ofEffect(move(effect));
}

Variable effectSeeInvisible(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newSeeInvisible();
    return Variable::ofEffect(move(effect));
}

Variable effectTimeStop(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newTimeStop();
    return Variable::ofEffect(move(effect));
}

Variable effectBlasterDeflectionIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newBlasterDeflectionIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectBlasterDeflectionDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newBlasterDeflectionDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectSpellLevelAbsorption(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newSpellLevelAbsorption();
    return Variable::ofEffect(move(effect));
}

Variable effectDispelMagicBest(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDispelMagicBest();
    return Variable::ofEffect(move(effect));
}

Variable effectMissChance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newMissChance();
    return Variable::ofEffect(move(effect));
}

Variable effectModifyAttacks(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newModifyAttacks();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageShield(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDamageShield();
    return Variable::ofEffect(move(effect));
}

Variable setEffectIcon(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectForceDrain(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForceDrain();
    return Variable::ofEffect(move(effect));
}

Variable effectPsychicStatic(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newPsychicStatic();
    return Variable::ofEffect(move(effect));
}

Variable effectLightsaberThrow(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newLightsaberThrow();
    return Variable::ofEffect(move(effect));
}

Variable effectWhirlWind(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newWhirlWind();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newCutsceneHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newCutsceneParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newCutsceneStunned();
    return Variable::ofEffect(move(effect));
}

// TSL

Variable effectForceBody(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForceBody();
    return Variable::ofEffect(move(effect));
}

Variable effectFury(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newFury();
    return Variable::ofEffect(move(effect));
}

Variable effectBlind(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newBlind();
    return Variable::ofEffect(move(effect));
}

Variable effectFPRegenModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newFPRegenModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectVPRegenModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newVPRegenModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectCrush(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newCrush();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidConfused(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDroidConfused();
    return Variable::ofEffect(move(effect));
}

Variable effectForceSight(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newForceSight();
    return Variable::ofEffect(move(effect));
}

Variable effectMindTrick(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newMindTrick();
    return Variable::ofEffect(move(effect));
}

Variable effectFactionModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newFactionModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidScramble(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.services.effectFactory.newDroidScramble();
    return Variable::ofEffect(move(effect));
}

// END TSL

} // namespace routine

} // namespace game

} // namespace reone
