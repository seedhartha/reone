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

#include "../../../effect/effect.h"
#include "../../../effect/factory.h"
#include "../../../game.h"
#include "../../../types.h"

#include "argutil.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable effectAssuredHit(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newAssuredHit();
    return Variable::ofEffect(move(effect));
}

Variable effectHeal(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newHeal();
    return Variable::ofEffect(move(effect));
}

Variable effectDamage(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDamage(0, DamageType::Universal, nullptr);
    return Variable::ofEffect(move(effect));
}

Variable effectAbilityIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newAbilityIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageResistance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDamageResistance();
    return Variable::ofEffect(move(effect));
}

Variable effectResurrection(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newResurrection();
    return Variable::ofEffect(move(effect));
}

Variable getIsEffectValid(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    try {
        auto effect = getEffect(args, 0);
        return Variable::ofInt(1);
    } catch (const ArgumentException &) {
        return Variable::ofInt(0);
    }
}

Variable getEffectDurationType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getEffectSubType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable getEffectCreator(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable magicalEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable supernaturalEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable extraordinaryEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable effectACIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newACIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newSavingThrowIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectAttackIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newAttackIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageReduction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDamageReduction();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDamageIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectEntangle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newEntangle();
    return Variable::ofEffect(move(effect));
}

Variable effectDeath(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDeath();
    return Variable::ofEffect(std::move(effect));
}

Variable effectKnockdown(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newKnockdown();
    return Variable::ofEffect(move(effect));
}

Variable effectParalyze(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectSpellImmunity(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newSpellImmunity();
    return Variable::ofEffect(move(effect));
}

Variable effectForceJump(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForceJump();
    return Variable::ofEffect(move(effect));
}

Variable effectSleep(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newSleep();
    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newTemporaryForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectConfused(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newConfused();
    return Variable::ofEffect(move(effect));
}

Variable effectFrightened(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newFrightened();
    return Variable::ofEffect(move(effect));
}

Variable effectChoke(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newChoke();
    return Variable::ofEffect(move(effect));
}

Variable effectStunned(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newStunned();
    return Variable::ofEffect(move(effect));
}

Variable effectRegenerate(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newRegenerate();
    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newMovementSpeedIncrease();
    return Variable::ofEffect(move(effect));
}

Variable getEffectType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable effectAreaOfEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newAreaOfEffect();
    return Variable::ofEffect(move(effect));
}

Variable effectVisualEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newVisual();
    return Variable::ofEffect(move(effect));
}

Variable effectLinkEffects(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newLinkEffects();
    return Variable::ofEffect(move(effect));
}

Variable effectBeam(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newBeam();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResistanceIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForceResistanceIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectBodyFuel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newBodyFuel();
    return Variable::ofEffect(move(effect));
}

Variable effectPoison(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newPoison();
    return Variable::ofEffect(move(effect));
}

Variable effectAssuredDeflection(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newAssuredDeflection();
    return Variable::ofEffect(move(effect));
}

Variable effectForcePushTargeted(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForcePushTargeted();
    return Variable::ofEffect(move(effect));
}

Variable effectHaste(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newHaste();
    return Variable::ofEffect(move(effect));
}

Variable effectImmunity(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newImmunity();
    return Variable::ofEffect(move(effect));
}

Variable getEffectSpellId(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable effectDamageImmunityIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDamageImmunityIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryHitpoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newTemporaryHitPoints();
    return Variable::ofEffect(move(effect));
}

Variable effectSkillIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newSkillIncrease();
    return Variable::ofEffect(move(effect));
}

Variable versusAlignmentEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable versusRacialTypeEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable versusTrapEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable effectDamageForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDamageForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectHealForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newHealForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectHitPointChangeWhenDying(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newHitPointsChangeWhenDying();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidStun(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDroidStun();
    return Variable::ofEffect(move(effect));
}

Variable effectForcePushed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForcePushed();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResisted(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForceResisted();
    return Variable::ofEffect(move(effect));
}

Variable effectForceFizzle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForceFizzle();
    return Variable::ofEffect(move(effect));
}

Variable effectAbilityDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newAbilityDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectAttackDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newAttackDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDamageDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageImmunityDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDamageImmunityDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectACDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newACDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newMovementSpeedDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newSavingThrowDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSkillDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newSkillDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResistanceDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForceResistanceDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectInvisibility(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newInvisibility();
    return Variable::ofEffect(move(effect));
}

Variable effectConcealment(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newConcealment();
    return Variable::ofEffect(move(effect));
}

Variable effectForceShield(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForceShield();
    return Variable::ofEffect(move(effect));
}

Variable effectDispelMagicAll(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDispelMagicAll();
    return Variable::ofEffect(move(effect));
}

Variable effectDisguise(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDisguise();
    return Variable::ofEffect(move(effect));
}

Variable effectTrueSeeing(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newTrueSeeing();
    return Variable::ofEffect(move(effect));
}

Variable effectSeeInvisible(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newSeeInvisible();
    return Variable::ofEffect(move(effect));
}

Variable effectTimeStop(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newTimeStop();
    return Variable::ofEffect(move(effect));
}

Variable effectBlasterDeflectionIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newBlasterDeflectionIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectBlasterDeflectionDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newBlasterDeflectionDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectHorrified(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectSpellLevelAbsorption(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newSpellLevelAbsorption();
    return Variable::ofEffect(move(effect));
}

Variable effectDispelMagicBest(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDispelMagicBest();
    return Variable::ofEffect(move(effect));
}

Variable effectMissChance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newMissChance();
    return Variable::ofEffect(move(effect));
}

Variable effectModifyAttacks(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newModifyAttacks();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageShield(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDamageShield();
    return Variable::ofEffect(move(effect));
}

Variable setEffectIcon(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable effectForceDrain(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForceDrain();
    return Variable::ofEffect(move(effect));
}

Variable effectPsychicStatic(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newPsychicStatic();
    return Variable::ofEffect(move(effect));
}

Variable effectLightsaberThrow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newLightsaberThrow();
    return Variable::ofEffect(move(effect));
}

Variable effectWhirlWind(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newWhirlWind();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneHorrified(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newCutsceneHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneParalyze(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newCutsceneParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneStunned(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newCutsceneStunned();
    return Variable::ofEffect(move(effect));
}

Variable effectForceBody(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForceBody();
    return Variable::ofEffect(move(effect));
}

Variable effectFury(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newFury();
    return Variable::ofEffect(move(effect));
}

Variable effectBlind(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newBlind();
    return Variable::ofEffect(move(effect));
}

Variable effectFPRegenModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newFPRegenModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectVPRegenModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newVPRegenModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectCrush(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newCrush();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidConfused(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDroidConfused();
    return Variable::ofEffect(move(effect));
}

Variable effectForceSight(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newForceSight();
    return Variable::ofEffect(move(effect));
}

Variable effectMindTrick(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newMindTrick();
    return Variable::ofEffect(move(effect));
}

Variable effectFactionModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newFactionModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidScramble(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.effectFactory().newDroidScramble();
    return Variable::ofEffect(move(effect));
}

} // namespace routine

} // namespace game

} // namespace reone
