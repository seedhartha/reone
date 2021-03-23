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

#include "routines.h"

#include "../../common/log.h"

#include "../enginetype/effect.h"
#include "../object/spatial.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::effectAssuredHit(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AssuredHit);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectHeal(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Heal);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDamage(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Damage);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectAbilityIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AbilityIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDamageResistance(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageResistance);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectResurrection(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Resurrection);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectACIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ACIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectSavingThrowIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SavingThrowIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectAttackIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AttackIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDamageReduction(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageReduction);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDamageIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectEntangle(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Entangle);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDeath(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Death);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectKnockdown(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Knockdown);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectParalyze(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Paralyze);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectSpellImmunity(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SpellImmunity);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForceJump(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceJump);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectSleep(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Sleep);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectTemporaryForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TemporaryHitpoints);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectConfused(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Confused);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectFrightened(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Frightened);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectChoke(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Choke);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectStunned(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Stunned);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectRegenerate(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Regenerate);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectMovementSpeedIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MovementSpeedIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectAreaOfEffect(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AreaOfEffect);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectVisualEffect(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Visual);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectLinkEffects(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::LinkEffects);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectBeam(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Beam);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForceResistanceIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceResistanceIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectBodyFuel(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::BodyFuel);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectPoison(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Poison);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectAssuredDeflection(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AssuredDeflection);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForcePushTargeted(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForcePushTargeted);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectHaste(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Haste);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectImmunity(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Immunity);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDamageImmunityIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageImmunityIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectTemporaryHitpoints(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TemporaryHitpoints);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectSkillIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SkillIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDamageForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageForcePoints);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectHealForcePoints(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::HealForcePoints);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectHitPointChangeWhenDying(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::HitPointsChangeWhenDying);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDroidStun(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DroidStun);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForcePushed(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForcePushed);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForceResisted(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceResisted);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForceFizzle(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceFizzle);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectAbilityDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AbilityDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectAttackDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::AttackDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDamageDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDamageImmunityDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageImmunityDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectACDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ACDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectMovementSpeedDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MovementSpeedDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectSavingThrowDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SavingThrowDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectSkillDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SkillDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForceResistanceDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceResistanceDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectInvisibility(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Invisibility);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectConcealment(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Concealment);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForceShield(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceShield);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDispelMagicAll(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DispelMagicAll);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDisguise(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Disguise);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectTrueSeeing(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TrueSeeing);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectSeeInvisible(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SeeInvisible);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectTimeStop(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::TimeStop);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectBlasterDeflectionIncrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::BlasterDeflectionIncrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectBlasterDeflectionDecrease(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::BlasterDeflectionDecrease);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectHorrified(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Horrified);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectSpellLevelAbsorption(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::SpellLevelAbsorption);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDispelMagicBest(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DispelMagicBest);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectMissChance(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MissChance);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectModifyAttacks(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ModifyAttacks);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDamageShield(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DamageShield);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForceDrain(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceDrain);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectPsychicStatic(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::PsychicStatic);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectLightsaberThrow(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::LightsaberThrow);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectWhirlWind(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::WhirlWind);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectCutSceneHorrified(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::CutSceneHorrified);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectCutSceneParalyze(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::CutSceneParalyze);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectCutSceneStunned(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::CutSceneStunned);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForceBody(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceBody);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectFury(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Fury);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectBlind(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Blindness);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectFPRegenModifier(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::FPRegenModifier);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectVPRegenModifier(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::VPRegenModifier);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectCrush(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::Crush);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDroidConfused(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DroidConfused);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectForceSight(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::ForceSight);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectMindTrick(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::MindTrick);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectFactionModifier(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::FactionModifier);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::effectDroidScramble(const VariablesList &args, ExecutionContext &ctx) {
    auto effect = make_shared<Effect>(EffectType::DroidScramble);
    return Variable(VariableType::Effect, static_pointer_cast<EngineType>(effect));
}

Variable Routines::clearAllEffects(const VariablesList &args, ExecutionContext &ctx) {
    auto caller = getCallerAsSpatial(ctx);
    caller->clearAllEffects();
    return Variable();
}

Variable Routines::applyEffectToObject(const VariablesList &args, ExecutionContext &ctx) {
    auto durationType = static_cast<DurationType>(getInt(args, 0));
    auto effect = getEffect(args, 1);
    auto target = getSpatialObject(args, 2);
    float duration = getFloat(args, 3, 0.0f);

    if (target) {
        target->applyEffect(effect, durationType, duration);
    } else {
        warn("Script: applyEffectToObject: target is invalid");
    }

    return Variable();
}

} // namespace game

} // namespace reone
