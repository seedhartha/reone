/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#pragma once

#include "abilitydecrease.h"
#include "abilityincrease.h"
#include "acdecrease.h"
#include "acincrease.h"
#include "areaofeffect.h"
#include "assureddeflection.h"
#include "assuredhit.h"
#include "attackdecrease.h"
#include "attackincrease.h"
#include "beam.h"
#include "blasterdeflectiondecrease.h"
#include "blasterdeflectionincrease.h"
#include "blind.h"
#include "bodyfuel.h"
#include "choke.h"
#include "concealment.h"
#include "confused.h"
#include "crush.h"
#include "cutscenehorrified.h"
#include "cutsceneparalyze.h"
#include "cutscenestunned.h"
#include "damage.h"
#include "damagedecrease.h"
#include "damageforcepoints.h"
#include "damageimmunitydecrease.h"
#include "damageimmunityincrease.h"
#include "damageincrease.h"
#include "damagereduction.h"
#include "damageresistance.h"
#include "damageshield.h"
#include "death.h"
#include "disguise.h"
#include "dispelmagicall.h"
#include "dispelmagicbest.h"
#include "droidconfused.h"
#include "droidscramble.h"
#include "droidstun.h"
#include "entangle.h"
#include "factionmodifier.h"
#include "forcebody.h"
#include "forcedrain.h"
#include "forcefizzle.h"
#include "forcejump.h"
#include "forcepushed.h"
#include "forcepushtargeted.h"
#include "forceresistancedecrease.h"
#include "forceresistanceincrease.h"
#include "forceresisted.h"
#include "forceshield.h"
#include "forcesight.h"
#include "fpregenmodifier.h"
#include "frightened.h"
#include "fury.h"
#include "haste.h"
#include "heal.h"
#include "healforcepoints.h"
#include "hitpointchangewhendying.h"
#include "horrified.h"
#include "immunity.h"
#include "invisibility.h"
#include "knockdown.h"
#include "lightsaberthrow.h"
#include "linkeffects.h"
#include "mindtrick.h"
#include "misschance.h"
#include "modifyattacks.h"
#include "movementspeeddecrease.h"
#include "movementspeedincrease.h"
#include "paralyze.h"
#include "poison.h"
#include "psychicstatic.h"
#include "regenerate.h"
#include "resurrection.h"
#include "savingthrowdecrease.h"
#include "savingthrowincrease.h"
#include "seeinvisible.h"
#include "skilldecrease.h"
#include "skillincrease.h"
#include "sleep.h"
#include "spellimmunity.h"
#include "spelllevelabsorption.h"
#include "stunned.h"
#include "temporaryforcepoints.h"
#include "temporaryhitpoints.h"
#include "timestop.h"
#include "trueseeing.h"
#include "visual.h"
#include "vpregenmodifier.h"
#include "whirlwind.h"

namespace reone {

namespace game {

class EffectFactory {
public:
    std::unique_ptr<AbilityDecreaseEffect> newAbilityDecrease(Ability ability, int modifyBy) {
        return std::make_unique<AbilityDecreaseEffect>(ability, modifyBy);
    }

    std::unique_ptr<AbilityIncreaseEffect> newAbilityIncrease(Ability abilityToIncrease, int modifyBy) {
        return std::make_unique<AbilityIncreaseEffect>(abilityToIncrease, modifyBy);
    }

    std::unique_ptr<ACDecreaseEffect> newACDecrease(int value, ACBonus modifyType, int damageType) {
        return std::make_unique<ACDecreaseEffect>(value, modifyType, damageType);
    }

    std::unique_ptr<ACIncreaseEffect> newACIncrease(int value, ACBonus modifyType, int damageType) {
        return std::make_unique<ACIncreaseEffect>(value, modifyType, damageType);
    }

    std::unique_ptr<AreaOfEffectEffect> newAreaOfEffect(int areaEffectId, std::string onEnterScript, std::string heartbeatScript, std::string onExitScript) {
        return std::make_unique<AreaOfEffectEffect>(areaEffectId, std::move(onEnterScript), std::move(heartbeatScript), std::move(onExitScript));
    }

    std::unique_ptr<AssuredDeflectionEffect> newAssuredDeflection(int returnDamage) {
        return std::make_unique<AssuredDeflectionEffect>(returnDamage);
    }

    std::unique_ptr<AssuredHitEffect> newAssuredHit() {
        return std::make_unique<AssuredHitEffect>();
    }

    std::unique_ptr<AttackDecreaseEffect> newAttackDecrease(int penalty, AttackBonus modifierType) {
        return std::make_unique<AttackDecreaseEffect>(penalty, modifierType);
    }

    std::unique_ptr<AttackIncreaseEffect> newAttackIncrease(int bonus, AttackBonus modifierType) {
        return std::make_unique<AttackIncreaseEffect>(bonus, modifierType);
    }

    std::unique_ptr<BeamEffect> newBeam(int beamVisualEffect, std::shared_ptr<Object> effector, BodyNode bodyPart, bool missEffect) {
        return std::make_unique<BeamEffect>(beamVisualEffect, std::move(effector), bodyPart, missEffect);
    }

    std::unique_ptr<BlasterDeflectionDecreaseEffect> newBlasterDeflectionDecrease(int change) {
        return std::make_unique<BlasterDeflectionDecreaseEffect>(change);
    }

    std::unique_ptr<BlasterDeflectionIncreaseEffect> newBlasterDeflectionIncrease(int change) {
        return std::make_unique<BlasterDeflectionIncreaseEffect>(change);
    }

    std::unique_ptr<BlindEffect> newBlind() {
        return std::make_unique<BlindEffect>();
    }

    std::unique_ptr<BodyFuelEffect> newBodyFuel() {
        return std::make_unique<BodyFuelEffect>();
    }

    std::unique_ptr<ChokeEffect> newChoke() {
        return std::make_unique<ChokeEffect>();
    }

    std::unique_ptr<ConcealmentEffect> newConcealment(int percentage) {
        return std::make_unique<ConcealmentEffect>(percentage);
    }

    std::unique_ptr<ConfusedEffect> newConfused() {
        return std::make_unique<ConfusedEffect>();
    }

    std::unique_ptr<CrushEffect> newCrush() {
        return std::make_unique<CrushEffect>();
    }

    std::unique_ptr<CutsceneHorrifiedEffect> newCutsceneHorrified() {
        return std::make_unique<CutsceneHorrifiedEffect>();
    }

    std::unique_ptr<CutsceneParalyzeEffect> newCutsceneParalyze() {
        return std::make_unique<CutsceneParalyzeEffect>();
    }

    std::unique_ptr<CutsceneStunnedEffect> newCutsceneStunned() {
        return std::make_unique<CutsceneStunnedEffect>();
    }

    std::unique_ptr<DamageEffect> newDamage(int amount, DamageType type, DamagePower power, std::shared_ptr<Creature> damager) {
        return std::make_unique<DamageEffect>(amount, type, power, std::move(damager));
    }

    std::unique_ptr<DamageDecreaseEffect> newDamageDecrease(int penalty, DamageType damageType) {
        return std::make_unique<DamageDecreaseEffect>(penalty, damageType);
    }

    std::unique_ptr<DamageForcePointsEffect> newDamageForcePoints(int damage) {
        return std::make_unique<DamageForcePointsEffect>(damage);
    }

    std::unique_ptr<DamageImmunityDecreaseEffect> newDamageImmunityDecrease(DamageType damageType, int percentImmunity) {
        return std::make_unique<DamageImmunityDecreaseEffect>(damageType, percentImmunity);
    }

    std::unique_ptr<DamageImmunityIncreaseEffect> newDamageImmunityIncrease(DamageType damageType, int percentImmunity) {
        return std::make_unique<DamageImmunityIncreaseEffect>(damageType, percentImmunity);
    }

    std::unique_ptr<DamageIncreaseEffect> newDamageIncrease(int bonus, DamageType damageType) {
        return std::make_unique<DamageIncreaseEffect>(bonus, damageType);
    }

    std::unique_ptr<DamageReductionEffect> newDamageReduction(int amount, DamagePower damagePower, int limit) {
        return std::make_unique<DamageReductionEffect>(amount, damagePower, limit);
    }

    std::unique_ptr<DamageResistanceEffect> newDamageResistance(DamageType damageType, int amount, int limit) {
        return std::make_unique<DamageResistanceEffect>(damageType, amount, limit);
    }

    std::unique_ptr<DamageShieldEffect> newDamageShield(int damageAmount, int randomAmount, DamageType damageType) {
        return std::make_unique<DamageShieldEffect>(damageAmount, randomAmount, damageType);
    }

    std::unique_ptr<DeathEffect> newDeath(bool spectacularDeath, bool displayFeedback, bool noFadeAway) {
        return std::make_unique<DeathEffect>(spectacularDeath, displayFeedback, noFadeAway);
    }

    std::unique_ptr<DisguiseEffect> newDisguise(int appearance) {
        return std::make_unique<DisguiseEffect>(appearance);
    }

    std::unique_ptr<DispelMagicAllEffect> newDispelMagicAll(int casterLevel) {
        return std::make_unique<DispelMagicAllEffect>(casterLevel);
    }

    std::unique_ptr<DispelMagicBestEffect> newDispelMagicBest(int casterLevel) {
        return std::make_unique<DispelMagicBestEffect>(casterLevel);
    }

    std::unique_ptr<DroidConfusedEffect> newDroidConfused() {
        return std::make_unique<DroidConfusedEffect>();
    }

    std::unique_ptr<DroidScrambleEffect> newDroidScramble() {
        return std::make_unique<DroidScrambleEffect>();
    }

    std::unique_ptr<DroidStunEffect> newDroidStun() {
        return std::make_unique<DroidStunEffect>();
    }

    std::unique_ptr<EntangleEffect> newEntangle() {
        return std::make_unique<EntangleEffect>();
    }

    std::unique_ptr<FactionModifierEffect> newFactionModifier(int newFaction) {
        return std::make_unique<FactionModifierEffect>(newFaction);
    }

    std::unique_ptr<ForceBodyEffect> newForceBody(int level) {
        return std::make_unique<ForceBodyEffect>(level);
    }

    std::unique_ptr<ForceDrainEffect> newForceDrain(int damage) {
        return std::make_unique<ForceDrainEffect>(damage);
    }

    std::unique_ptr<ForceFizzleEffect> newForceFizzle() {
        return std::make_unique<ForceFizzleEffect>();
    }

    std::unique_ptr<ForceJumpEffect> newForceJump(std::shared_ptr<Object> target, int advanced) {
        return std::make_unique<ForceJumpEffect>(std::move(target), advanced);
    }

    std::unique_ptr<ForcePushedEffect> newForcePushed() {
        return std::make_unique<ForcePushedEffect>();
    }

    std::unique_ptr<ForcePushTargetedEffect> newForcePushTargeted(std::shared_ptr<Location> centre, bool ignoreTestDirectLine) {
        return std::make_unique<ForcePushTargetedEffect>(std::move(centre), ignoreTestDirectLine);
    }

    std::unique_ptr<ForceResistanceDecreaseEffect> newForceResistanceDecrease(int value) {
        return std::make_unique<ForceResistanceDecreaseEffect>(value);
    }

    std::unique_ptr<ForceResistanceIncreaseEffect> newForceResistanceIncrease(int value) {
        return std::make_unique<ForceResistanceIncreaseEffect>(value);
    }

    std::unique_ptr<ForceResistedEffect> newForceResisted(Object &source) {
        return std::make_unique<ForceResistedEffect>(source);
    }

    std::unique_ptr<ForceShieldEffect> newForceShield(int shield) {
        return std::make_unique<ForceShieldEffect>(shield);
    }

    std::unique_ptr<ForceSightEffect> newForceSight() {
        return std::make_unique<ForceSightEffect>();
    }

    std::unique_ptr<FPRegenModifierEffect> newFPRegenModifier(int percent) {
        return std::make_unique<FPRegenModifierEffect>(percent);
    }

    std::unique_ptr<FrightenedEffect> newFrightened() {
        return std::make_unique<FrightenedEffect>();
    }

    std::unique_ptr<FuryEffect> newFury() {
        return std::make_unique<FuryEffect>();
    }

    std::unique_ptr<HasteEffect> newHaste() {
        return std::make_unique<HasteEffect>();
    }

    std::unique_ptr<HealEffect> newHeal(int damageToHeal) {
        return std::make_unique<HealEffect>(damageToHeal);
    }

    std::unique_ptr<HealForcePointsEffect> newHealForcePoints(int heal) {
        return std::make_unique<HealForcePointsEffect>(heal);
    }

    std::unique_ptr<HitPointChangeWhenDyingEffect> newHitPointChangeWhenDying(float hitPointChangePerRound) {
        return std::make_unique<HitPointChangeWhenDyingEffect>(hitPointChangePerRound);
    }

    std::unique_ptr<HorrifiedEffect> newHorrified() {
        return std::make_unique<HorrifiedEffect>();
    }

    std::unique_ptr<ImmunityEffect> newImmunity(ImmunityType immunityType) {
        return std::make_unique<ImmunityEffect>(immunityType);
    }

    std::unique_ptr<InvisibilityEffect> newInvisibility(InvisibilityType type) {
        return std::make_unique<InvisibilityEffect>(type);
    }

    std::unique_ptr<KnockdownEffect> newKnockdown() {
        return std::make_unique<KnockdownEffect>();
    }

    std::unique_ptr<LightsaberThrowEffect> newLightsaberThrow(std::shared_ptr<Object> target1, std::shared_ptr<Object> target2, std::shared_ptr<Object> target3, int advancedDamage) {
        return std::make_unique<LightsaberThrowEffect>(std::move(target1), std::move(target2), std::move(target3), advancedDamage);
    }

    std::unique_ptr<LinkEffectsEffect> newLinkEffects(std::shared_ptr<Effect> childEffect, std::shared_ptr<Effect> parentEffect) {
        return std::make_unique<LinkEffectsEffect>(std::move(childEffect), std::move(parentEffect));
    }

    std::unique_ptr<MindTrickEffect> newMindTrick() {
        return std::make_unique<MindTrickEffect>();
    }

    std::unique_ptr<MissChanceEffect> newMissChance(int percentage) {
        return std::make_unique<MissChanceEffect>(percentage);
    }

    std::unique_ptr<ModifyAttacksEffect> newModifyAttacks(int attacks) {
        return std::make_unique<ModifyAttacksEffect>(attacks);
    }

    std::unique_ptr<MovementSpeedDecreaseEffect> newMovementSpeedDecrease(int percentChange) {
        return std::make_unique<MovementSpeedDecreaseEffect>(percentChange);
    }

    std::unique_ptr<MovementSpeedIncreaseEffect> newMovementSpeedIncrease(int newSpeedPercent) {
        return std::make_unique<MovementSpeedIncreaseEffect>(newSpeedPercent);
    }

    std::unique_ptr<ParalyzeEffect> newParalyze() {
        return std::make_unique<ParalyzeEffect>();
    }

    std::unique_ptr<PoisonEffect> newPoison(Poison poisonType) {
        return std::make_unique<PoisonEffect>(poisonType);
    }

    std::unique_ptr<PsychicStaticEffect> newPsychicStatic() {
        return std::make_unique<PsychicStaticEffect>();
    }

    std::unique_ptr<RegenerateEffect> newRegenerate(int amount, float intervalSeconds) {
        return std::make_unique<RegenerateEffect>(amount, intervalSeconds);
    }

    std::unique_ptr<ResurrectionEffect> newResurrection(int hpPercent) {
        return std::make_unique<ResurrectionEffect>(hpPercent);
    }

    std::unique_ptr<SavingThrowDecreaseEffect> newSavingThrowDecrease(int save, int value, SavingThrowType savingThrowType) {
        return std::make_unique<SavingThrowDecreaseEffect>(save, value, savingThrowType);
    }

    std::unique_ptr<SavingThrowIncreaseEffect> newSavingThrowIncrease(int save, int value, SavingThrowType savingThrowType) {
        return std::make_unique<SavingThrowIncreaseEffect>(save, value, savingThrowType);
    }

    std::unique_ptr<SeeInvisibleEffect> newSeeInvisible() {
        return std::make_unique<SeeInvisibleEffect>();
    }

    std::unique_ptr<SkillDecreaseEffect> newSkillDecrease(SkillType skill, int value) {
        return std::make_unique<SkillDecreaseEffect>(skill, value);
    }

    std::unique_ptr<SkillIncreaseEffect> newSkillIncrease(SkillType skill, int value) {
        return std::make_unique<SkillIncreaseEffect>(skill, value);
    }

    std::unique_ptr<SleepEffect> newSleep() {
        return std::make_unique<SleepEffect>();
    }

    std::unique_ptr<SpellImmunityEffect> newSpellImmunity(SpellType immunityToSpell) {
        return std::make_unique<SpellImmunityEffect>(immunityToSpell);
    }

    std::unique_ptr<SpellLevelAbsorptionEffect> newSpellLevelAbsorption(int maxSpellLevelAbsorbed, int totalSpellLevelsAbsorbed, int spellSchool) {
        return std::make_unique<SpellLevelAbsorptionEffect>(maxSpellLevelAbsorbed, totalSpellLevelsAbsorbed, spellSchool);
    }

    std::unique_ptr<StunnedEffect> newStunned() {
        return std::make_unique<StunnedEffect>();
    }

    std::unique_ptr<TemporaryForcePointsEffect> newTemporaryForcePoints(int tempForce) {
        return std::make_unique<TemporaryForcePointsEffect>(tempForce);
    }

    std::unique_ptr<TemporaryHitPointsEffect> newTemporaryHitPoints(int hitPoints) {
        return std::make_unique<TemporaryHitPointsEffect>(hitPoints);
    }

    std::unique_ptr<TimeStopEffect> newTimeStop() {
        return std::make_unique<TimeStopEffect>();
    }

    std::unique_ptr<TrueSeeingEffect> newTrueSeeing() {
        return std::make_unique<TrueSeeingEffect>();
    }

    std::unique_ptr<VisualEffect> newVisual(int visualEffectId, bool missEffect) {
        return std::make_unique<VisualEffect>(visualEffectId, missEffect);
    }

    std::unique_ptr<VPRegenModifierEffect> newVPRegenModifier(int percent) {
        return std::make_unique<VPRegenModifierEffect>(percent);
    }

    std::unique_ptr<WhirlWindEffect> newWhirlWind() {
        return std::make_unique<WhirlWindEffect>();
    }
};

} // namespace game

} // namespace reone
