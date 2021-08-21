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

#include "../../routines.h"

#include "../../../game.h"
#include "../../../effect/effect.h"
#include "../../../types.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::effectAssuredHit(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AssuredHit);
    return Variable::ofEffect(effect);
}

Variable Routines::effectHeal(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Heal);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDamage(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = _game.services().effectFactory().newDamage(0, DamageType::Universal, nullptr);
    return Variable::ofEffect(std::move(effect));
}

Variable Routines::effectAbilityIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AbilityIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDamageResistance(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageResistance);
    return Variable::ofEffect(effect);
}

Variable Routines::effectResurrection(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Resurrection);
    return Variable::ofEffect(effect);
}

Variable Routines::getIsEffectValid(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getEffectDurationType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getEffectSubType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::getEffectCreator(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::magicalEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::supernaturalEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::extraordinaryEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::effectACIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ACIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectSavingThrowIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SavingThrowIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectAttackIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AttackIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDamageReduction(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageReduction);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDamageIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectEntangle(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Entangle);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDeath(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = _game.services().effectFactory().newDeath();
    return Variable::ofEffect(std::move(effect));
}

Variable Routines::effectKnockdown(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Knockdown);
    return Variable::ofEffect(effect);
}

Variable Routines::effectParalyze(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Paralyze);
    return Variable::ofEffect(effect);
}

Variable Routines::effectSpellImmunity(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SpellImmunity);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForceJump(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceJump);
    return Variable::ofEffect(effect);
}

Variable Routines::effectSleep(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Sleep);
    return Variable::ofEffect(effect);
}

Variable Routines::effectTemporaryForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TemporaryHitpoints);
    return Variable::ofEffect(effect);
}

Variable Routines::effectConfused(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Confused);
    return Variable::ofEffect(effect);
}

Variable Routines::effectFrightened(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Frightened);
    return Variable::ofEffect(effect);
}

Variable Routines::effectChoke(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Choke);
    return Variable::ofEffect(effect);
}

Variable Routines::effectStunned(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Stunned);
    return Variable::ofEffect(effect);
}

Variable Routines::effectRegenerate(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Regenerate);
    return Variable::ofEffect(effect);
}

Variable Routines::effectMovementSpeedIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MovementSpeedIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::getEffectType(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::effectAreaOfEffect(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AreaOfEffect);
    return Variable::ofEffect(effect);
}

Variable Routines::effectVisualEffect(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Visual);
    return Variable::ofEffect(effect);
}

Variable Routines::effectLinkEffects(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::LinkEffects);
    return Variable::ofEffect(effect);
}

Variable Routines::effectBeam(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Beam);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForceResistanceIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceResistanceIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectBodyFuel(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::BodyFuel);
    return Variable::ofEffect(effect);
}

Variable Routines::effectPoison(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Poison);
    return Variable::ofEffect(effect);
}

Variable Routines::effectAssuredDeflection(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AssuredDeflection);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForcePushTargeted(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForcePushTargeted);
    return Variable::ofEffect(effect);
}

Variable Routines::effectHaste(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Haste);
    return Variable::ofEffect(effect);
}

Variable Routines::effectImmunity(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Immunity);
    return Variable::ofEffect(effect);
}

Variable Routines::getEffectSpellId(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::effectDamageImmunityIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageImmunityIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectTemporaryHitpoints(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TemporaryHitpoints);
    return Variable::ofEffect(effect);
}

Variable Routines::effectSkillIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SkillIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::versusAlignmentEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::versusRacialTypeEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::versusTrapEffect(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::effectDamageForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageForcePoints);
    return Variable::ofEffect(effect);
}

Variable Routines::effectHealForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::HealForcePoints);
    return Variable::ofEffect(effect);
}

Variable Routines::effectHitPointChangeWhenDying(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::HitPointsChangeWhenDying);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDroidStun(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DroidStun);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForcePushed(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForcePushed);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForceResisted(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceResisted);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForceFizzle(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceFizzle);
    return Variable::ofEffect(effect);
}

Variable Routines::effectAbilityDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AbilityDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectAttackDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AttackDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDamageDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDamageImmunityDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageImmunityDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectACDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ACDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectMovementSpeedDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MovementSpeedDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectSavingThrowDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SavingThrowDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectSkillDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SkillDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForceResistanceDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceResistanceDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectInvisibility(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Invisibility);
    return Variable::ofEffect(effect);
}

Variable Routines::effectConcealment(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Concealment);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForceShield(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceShield);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDispelMagicAll(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DispelMagicAll);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDisguise(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Disguise);
    return Variable::ofEffect(effect);
}

Variable Routines::effectTrueSeeing(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TrueSeeing);
    return Variable::ofEffect(effect);
}

Variable Routines::effectSeeInvisible(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SeeInvisible);
    return Variable::ofEffect(effect);
}

Variable Routines::effectTimeStop(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TimeStop);
    return Variable::ofEffect(effect);
}

Variable Routines::effectBlasterDeflectionIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::BlasterDeflectionIncrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectBlasterDeflectionDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::BlasterDeflectionDecrease);
    return Variable::ofEffect(effect);
}

Variable Routines::effectHorrified(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Horrified);
    return Variable::ofEffect(effect);
}

Variable Routines::effectSpellLevelAbsorption(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SpellLevelAbsorption);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDispelMagicBest(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DispelMagicBest);
    return Variable::ofEffect(effect);
}

Variable Routines::effectMissChance(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MissChance);
    return Variable::ofEffect(effect);
}

Variable Routines::effectModifyAttacks(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ModifyAttacks);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDamageShield(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageShield);
    return Variable::ofEffect(effect);
}

Variable Routines::setEffectIcon(const VariablesList &args, ExecutionContext &ctx) {
    return Variable::notImplemented();
}

Variable Routines::effectForceDrain(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceDrain);
    return Variable::ofEffect(effect);
}

Variable Routines::effectPsychicStatic(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::PsychicStatic);
    return Variable::ofEffect(effect);
}

Variable Routines::effectLightsaberThrow(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::LightsaberThrow);
    return Variable::ofEffect(effect);
}

Variable Routines::effectWhirlWind(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::WhirlWind);
    return Variable::ofEffect(effect);
}

Variable Routines::effectCutSceneHorrified(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::CutSceneHorrified);
    return Variable::ofEffect(effect);
}

Variable Routines::effectCutSceneParalyze(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::CutSceneParalyze);
    return Variable::ofEffect(effect);
}

Variable Routines::effectCutSceneStunned(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::CutSceneStunned);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForceBody(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceBody);
    return Variable::ofEffect(effect);
}

Variable Routines::effectFury(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Fury);
    return Variable::ofEffect(effect);
}

Variable Routines::effectBlind(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Blindness);
    return Variable::ofEffect(effect);
}

Variable Routines::effectFPRegenModifier(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::FPRegenModifier);
    return Variable::ofEffect(effect);
}

Variable Routines::effectVPRegenModifier(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::VPRegenModifier);
    return Variable::ofEffect(effect);
}

Variable Routines::effectCrush(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Crush);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDroidConfused(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DroidConfused);
    return Variable::ofEffect(effect);
}

Variable Routines::effectForceSight(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceSight);
    return Variable::ofEffect(effect);
}

Variable Routines::effectMindTrick(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MindTrick);
    return Variable::ofEffect(effect);
}

Variable Routines::effectFactionModifier(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::FactionModifier);
    return Variable::ofEffect(effect);
}

Variable Routines::effectDroidScramble(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DroidScramble);
    return Variable::ofEffect(effect);
}

} // namespace game

} // namespace reone
