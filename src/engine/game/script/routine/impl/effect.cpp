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

#include "../../../../script/exception/notimpl.h"

#include "../../../game.h"
#include "../../../effect/effect.h"
#include "../../../types.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable effectAssuredHit(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AssuredHit);
    return Variable::ofEffect(effect);
}

Variable effectHeal(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Heal);
    return Variable::ofEffect(effect);
}

Variable effectDamage(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.services().effectFactory().newDamage(0, DamageType::Universal, nullptr);
    return Variable::ofEffect(std::move(effect));
}

Variable effectAbilityIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AbilityIncrease);
    return Variable::ofEffect(effect);
}

Variable effectDamageResistance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageResistance);
    return Variable::ofEffect(effect);
}

Variable effectResurrection(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Resurrection);
    return Variable::ofEffect(effect);
}

Variable getIsEffectValid(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
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
    auto effect = make_shared<Effect>(EffectType::ACIncrease);
    return Variable::ofEffect(effect);
}

Variable effectSavingThrowIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SavingThrowIncrease);
    return Variable::ofEffect(effect);
}

Variable effectAttackIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AttackIncrease);
    return Variable::ofEffect(effect);
}

Variable effectDamageReduction(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageReduction);
    return Variable::ofEffect(effect);
}

Variable effectDamageIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageIncrease);
    return Variable::ofEffect(effect);
}

Variable effectEntangle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Entangle);
    return Variable::ofEffect(effect);
}

Variable effectDeath(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = game.services().effectFactory().newDeath();
    return Variable::ofEffect(std::move(effect));
}

Variable effectKnockdown(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Knockdown);
    return Variable::ofEffect(effect);
}

Variable effectParalyze(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Paralyze);
    return Variable::ofEffect(effect);
}

Variable effectSpellImmunity(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SpellImmunity);
    return Variable::ofEffect(effect);
}

Variable effectForceJump(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceJump);
    return Variable::ofEffect(effect);
}

Variable effectSleep(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Sleep);
    return Variable::ofEffect(effect);
}

Variable effectTemporaryForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TemporaryHitpoints);
    return Variable::ofEffect(effect);
}

Variable effectConfused(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Confused);
    return Variable::ofEffect(effect);
}

Variable effectFrightened(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Frightened);
    return Variable::ofEffect(effect);
}

Variable effectChoke(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Choke);
    return Variable::ofEffect(effect);
}

Variable effectStunned(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Stunned);
    return Variable::ofEffect(effect);
}

Variable effectRegenerate(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Regenerate);
    return Variable::ofEffect(effect);
}

Variable effectMovementSpeedIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MovementSpeedIncrease);
    return Variable::ofEffect(effect);
}

Variable getEffectType(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable effectAreaOfEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AreaOfEffect);
    return Variable::ofEffect(effect);
}

Variable effectVisualEffect(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Visual);
    return Variable::ofEffect(effect);
}

Variable effectLinkEffects(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::LinkEffects);
    return Variable::ofEffect(effect);
}

Variable effectBeam(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Beam);
    return Variable::ofEffect(effect);
}

Variable effectForceResistanceIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceResistanceIncrease);
    return Variable::ofEffect(effect);
}

Variable effectBodyFuel(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::BodyFuel);
    return Variable::ofEffect(effect);
}

Variable effectPoison(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Poison);
    return Variable::ofEffect(effect);
}

Variable effectAssuredDeflection(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AssuredDeflection);
    return Variable::ofEffect(effect);
}

Variable effectForcePushTargeted(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForcePushTargeted);
    return Variable::ofEffect(effect);
}

Variable effectHaste(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Haste);
    return Variable::ofEffect(effect);
}

Variable effectImmunity(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Immunity);
    return Variable::ofEffect(effect);
}

Variable getEffectSpellId(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable effectDamageImmunityIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageImmunityIncrease);
    return Variable::ofEffect(effect);
}

Variable effectTemporaryHitpoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TemporaryHitpoints);
    return Variable::ofEffect(effect);
}

Variable effectSkillIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SkillIncrease);
    return Variable::ofEffect(effect);
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
    auto effect = make_shared<Effect>(EffectType::DamageForcePoints);
    return Variable::ofEffect(effect);
}

Variable effectHealForcePoints(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::HealForcePoints);
    return Variable::ofEffect(effect);
}

Variable effectHitPointChangeWhenDying(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::HitPointsChangeWhenDying);
    return Variable::ofEffect(effect);
}

Variable effectDroidStun(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DroidStun);
    return Variable::ofEffect(effect);
}

Variable effectForcePushed(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForcePushed);
    return Variable::ofEffect(effect);
}

Variable effectForceResisted(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceResisted);
    return Variable::ofEffect(effect);
}

Variable effectForceFizzle(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceFizzle);
    return Variable::ofEffect(effect);
}

Variable effectAbilityDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AbilityDecrease);
    return Variable::ofEffect(effect);
}

Variable effectAttackDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AttackDecrease);
    return Variable::ofEffect(effect);
}

Variable effectDamageDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageDecrease);
    return Variable::ofEffect(effect);
}

Variable effectDamageImmunityDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageImmunityDecrease);
    return Variable::ofEffect(effect);
}

Variable effectACDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ACDecrease);
    return Variable::ofEffect(effect);
}

Variable effectMovementSpeedDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MovementSpeedDecrease);
    return Variable::ofEffect(effect);
}

Variable effectSavingThrowDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SavingThrowDecrease);
    return Variable::ofEffect(effect);
}

Variable effectSkillDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SkillDecrease);
    return Variable::ofEffect(effect);
}

Variable effectForceResistanceDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceResistanceDecrease);
    return Variable::ofEffect(effect);
}

Variable effectInvisibility(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Invisibility);
    return Variable::ofEffect(effect);
}

Variable effectConcealment(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Concealment);
    return Variable::ofEffect(effect);
}

Variable effectForceShield(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceShield);
    return Variable::ofEffect(effect);
}

Variable effectDispelMagicAll(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DispelMagicAll);
    return Variable::ofEffect(effect);
}

Variable effectDisguise(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Disguise);
    return Variable::ofEffect(effect);
}

Variable effectTrueSeeing(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TrueSeeing);
    return Variable::ofEffect(effect);
}

Variable effectSeeInvisible(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SeeInvisible);
    return Variable::ofEffect(effect);
}

Variable effectTimeStop(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TimeStop);
    return Variable::ofEffect(effect);
}

Variable effectBlasterDeflectionIncrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::BlasterDeflectionIncrease);
    return Variable::ofEffect(effect);
}

Variable effectBlasterDeflectionDecrease(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::BlasterDeflectionDecrease);
    return Variable::ofEffect(effect);
}

Variable effectHorrified(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Horrified);
    return Variable::ofEffect(effect);
}

Variable effectSpellLevelAbsorption(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SpellLevelAbsorption);
    return Variable::ofEffect(effect);
}

Variable effectDispelMagicBest(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DispelMagicBest);
    return Variable::ofEffect(effect);
}

Variable effectMissChance(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MissChance);
    return Variable::ofEffect(effect);
}

Variable effectModifyAttacks(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ModifyAttacks);
    return Variable::ofEffect(effect);
}

Variable effectDamageShield(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageShield);
    return Variable::ofEffect(effect);
}

Variable setEffectIcon(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    throw NotImplementedException();
}

Variable effectForceDrain(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceDrain);
    return Variable::ofEffect(effect);
}

Variable effectPsychicStatic(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::PsychicStatic);
    return Variable::ofEffect(effect);
}

Variable effectLightsaberThrow(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::LightsaberThrow);
    return Variable::ofEffect(effect);
}

Variable effectWhirlWind(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::WhirlWind);
    return Variable::ofEffect(effect);
}

Variable effectCutSceneHorrified(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::CutSceneHorrified);
    return Variable::ofEffect(effect);
}

Variable effectCutSceneParalyze(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::CutSceneParalyze);
    return Variable::ofEffect(effect);
}

Variable effectCutSceneStunned(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::CutSceneStunned);
    return Variable::ofEffect(effect);
}

Variable effectForceBody(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceBody);
    return Variable::ofEffect(effect);
}

Variable effectFury(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Fury);
    return Variable::ofEffect(effect);
}

Variable effectBlind(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Blindness);
    return Variable::ofEffect(effect);
}

Variable effectFPRegenModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::FPRegenModifier);
    return Variable::ofEffect(effect);
}

Variable effectVPRegenModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::VPRegenModifier);
    return Variable::ofEffect(effect);
}

Variable effectCrush(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Crush);
    return Variable::ofEffect(effect);
}

Variable effectDroidConfused(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DroidConfused);
    return Variable::ofEffect(effect);
}

Variable effectForceSight(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceSight);
    return Variable::ofEffect(effect);
}

Variable effectMindTrick(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MindTrick);
    return Variable::ofEffect(effect);
}

Variable effectFactionModifier(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::FactionModifier);
    return Variable::ofEffect(effect);
}

Variable effectDroidScramble(Game &game, const vector<Variable> &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DroidScramble);
    return Variable::ofEffect(effect);
}

} // namespace routine

} // namespace game

} // namespace reone
