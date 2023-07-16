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

#include "reone/game/effect/abilitydecrease.h"
#include "reone/game/effect/abilityincrease.h"
#include "reone/game/effect/acdecrease.h"
#include "reone/game/effect/acincrease.h"
#include "reone/game/effect/areaofeffect.h"
#include "reone/game/effect/assureddeflection.h"
#include "reone/game/effect/assuredhit.h"
#include "reone/game/effect/attackdecrease.h"
#include "reone/game/effect/attackincrease.h"
#include "reone/game/effect/beam.h"
#include "reone/game/effect/blasterdeflectiondecrease.h"
#include "reone/game/effect/blasterdeflectionincrease.h"
#include "reone/game/effect/blind.h"
#include "reone/game/effect/bodyfuel.h"
#include "reone/game/effect/choke.h"
#include "reone/game/effect/concealment.h"
#include "reone/game/effect/confused.h"
#include "reone/game/effect/crush.h"
#include "reone/game/effect/cutscenehorrified.h"
#include "reone/game/effect/cutsceneparalyze.h"
#include "reone/game/effect/cutscenestunned.h"
#include "reone/game/effect/damage.h"
#include "reone/game/effect/damagedecrease.h"
#include "reone/game/effect/damageforcepoints.h"
#include "reone/game/effect/damageimmunitydecrease.h"
#include "reone/game/effect/damageimmunityincrease.h"
#include "reone/game/effect/damageincrease.h"
#include "reone/game/effect/damagereduction.h"
#include "reone/game/effect/damageresistance.h"
#include "reone/game/effect/damageshield.h"
#include "reone/game/effect/death.h"
#include "reone/game/effect/disguise.h"
#include "reone/game/effect/dispelmagicall.h"
#include "reone/game/effect/dispelmagicbest.h"
#include "reone/game/effect/droidconfused.h"
#include "reone/game/effect/droidscramble.h"
#include "reone/game/effect/droidstun.h"
#include "reone/game/effect/entangle.h"
#include "reone/game/effect/factionmodifier.h"
#include "reone/game/effect/forcebody.h"
#include "reone/game/effect/forcedrain.h"
#include "reone/game/effect/forcefizzle.h"
#include "reone/game/effect/forcejump.h"
#include "reone/game/effect/forcepushed.h"
#include "reone/game/effect/forcepushtargeted.h"
#include "reone/game/effect/forceresistancedecrease.h"
#include "reone/game/effect/forceresistanceincrease.h"
#include "reone/game/effect/forceresisted.h"
#include "reone/game/effect/forceshield.h"
#include "reone/game/effect/forcesight.h"
#include "reone/game/effect/fpregenmodifier.h"
#include "reone/game/effect/frightened.h"
#include "reone/game/effect/fury.h"
#include "reone/game/effect/haste.h"
#include "reone/game/effect/heal.h"
#include "reone/game/effect/healforcepoints.h"
#include "reone/game/effect/hitpointchangewhendying.h"
#include "reone/game/effect/horrified.h"
#include "reone/game/effect/immunity.h"
#include "reone/game/effect/invisibility.h"
#include "reone/game/effect/knockdown.h"
#include "reone/game/effect/lightsaberthrow.h"
#include "reone/game/effect/linkeffects.h"
#include "reone/game/effect/mindtrick.h"
#include "reone/game/effect/misschance.h"
#include "reone/game/effect/modifyattacks.h"
#include "reone/game/effect/movementspeeddecrease.h"
#include "reone/game/effect/movementspeedincrease.h"
#include "reone/game/effect/paralyze.h"
#include "reone/game/effect/poison.h"
#include "reone/game/effect/psychicstatic.h"
#include "reone/game/effect/regenerate.h"
#include "reone/game/effect/resurrection.h"
#include "reone/game/effect/savingthrowdecrease.h"
#include "reone/game/effect/savingthrowincrease.h"
#include "reone/game/effect/seeinvisible.h"
#include "reone/game/effect/skilldecrease.h"
#include "reone/game/effect/skillincrease.h"
#include "reone/game/effect/sleep.h"
#include "reone/game/effect/spellimmunity.h"
#include "reone/game/effect/spelllevelabsorption.h"
#include "reone/game/effect/stunned.h"
#include "reone/game/effect/temporaryforcepoints.h"
#include "reone/game/effect/temporaryhitpoints.h"
#include "reone/game/effect/timestop.h"
#include "reone/game/effect/trueseeing.h"
#include "reone/game/effect/visual.h"
#include "reone/game/effect/vpregenmodifier.h"
#include "reone/game/effect/whirlwind.h"
#include "reone/game/game.h"
#include "reone/game/script/routine/argutil.h"
#include "reone/game/script/routine/context.h"
#include "reone/game/script/routines.h"
#include "reone/script/routine/exception/notimplemented.h"
#include "reone/script/variable.h"

#define R_VOID script::VariableType::Void
#define R_INT script::VariableType::Int
#define R_FLOAT script::VariableType::Float
#define R_OBJECT script::VariableType::Object
#define R_STRING script::VariableType::String
#define R_EFFECT script::VariableType::Effect
#define R_EVENT script::VariableType::Event
#define R_LOCATION script::VariableType::Location
#define R_TALENT script::VariableType::Talent
#define R_VECTOR script::VariableType::Vector
#define R_ACTION script::VariableType::Action

using namespace reone::script;

namespace reone {

namespace game {

static Variable EffectAssuredHit(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<AssuredHitEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectHeal(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamageToHeal = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<HealEffect>(nDamageToHeal);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDamage(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamageAmount = getInt(args, 0);
    auto nDamageType = getIntOrElse(args, 1, 8);
    auto nDamagePower = getIntOrElse(args, 2, 0);

    // Transform
    auto damageType = static_cast<DamageType>(nDamageType);
    auto damagePower = static_cast<DamagePower>(nDamagePower);

    // Execute
    auto effect = ctx.game.newEffect<DamageEffect>(nDamageAmount, damageType, damagePower, nullptr);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectAbilityIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAbilityToIncrease = getInt(args, 0);
    auto nModifyBy = getInt(args, 1);

    // Transform
    auto abilityToIncrease = static_cast<Ability>(nAbilityToIncrease);

    // Execute
    auto effect = ctx.game.newEffect<AbilityIncreaseEffect>(abilityToIncrease, nModifyBy);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDamageResistance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamageType = getInt(args, 0);
    auto nAmount = getInt(args, 1);
    auto nLimit = getIntOrElse(args, 2, 0);

    // Transform
    auto damageType = static_cast<DamageType>(nDamageType);

    // Execute
    auto effect = ctx.game.newEffect<DamageResistanceEffect>(damageType, nAmount, nLimit);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectResurrection(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nHPPercent = getIntOrElse(args, 0, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ResurrectionEffect>(nHPPercent);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectACIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nValue = getInt(args, 0);
    auto nModifyType = getIntOrElse(args, 1, 0);
    auto nDamageType = getIntOrElse(args, 2, 8199);

    // Transform
    auto modifyType = static_cast<ACBonus>(nModifyType);

    // Execute
    auto effect = ctx.game.newEffect<ACIncreaseEffect>(nValue, modifyType, nDamageType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectSavingThrowIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSave = getInt(args, 0);
    auto nValue = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);

    // Transform
    auto saveType = static_cast<SavingThrowType>(nSaveType);

    // Execute
    auto effect = ctx.game.newEffect<SavingThrowIncreaseEffect>(nSave, nValue, saveType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectAttackIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nBonus = getInt(args, 0);
    auto nModifierType = getIntOrElse(args, 1, 0);

    // Transform
    auto modifierType = static_cast<AttackBonus>(nModifierType);

    // Execute
    auto effect = ctx.game.newEffect<AttackIncreaseEffect>(nBonus, modifierType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDamageReduction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAmount = getInt(args, 0);
    auto nDamagePower = getInt(args, 1);
    auto nLimit = getIntOrElse(args, 2, 0);

    // Transform
    auto damagePower = static_cast<DamagePower>(nDamagePower);

    // Execute
    auto effect = ctx.game.newEffect<DamageReductionEffect>(nAmount, damagePower, nLimit);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDamageIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nBonus = getInt(args, 0);
    auto nDamageType = getIntOrElse(args, 1, 8);

    // Transform
    auto damageType = static_cast<DamageType>(nDamageType);

    // Execute
    auto effect = ctx.game.newEffect<DamageIncreaseEffect>(nBonus, damageType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectEntangle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<EntangleEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDeath(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpectacularDeath = getIntOrElse(args, 0, 0);
    auto nDisplayFeedback = getIntOrElse(args, 1, 1);
    auto nNoFadeAway = getIntOrElse(args, 2, 0);

    // Transform
    auto spectacularDeath = static_cast<bool>(nSpectacularDeath);
    auto displayFeedback = static_cast<bool>(nDisplayFeedback);
    auto noFadeAway = static_cast<bool>(nNoFadeAway);

    // Execute
    auto effect = ctx.game.newEffect<DeathEffect>(spectacularDeath, displayFeedback, noFadeAway);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectKnockdown(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<KnockdownEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectParalyze(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<ParalyzeEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectSpellImmunity(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nImmunityToSpell = getIntOrElse(args, 0, -1);

    // Transform
    auto immunityToSpell = static_cast<SpellType>(nImmunityToSpell);

    // Execute
    auto effect = ctx.game.newEffect<SpellImmunityEffect>(immunityToSpell);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForceJump(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto nAdvanced = getIntOrElse(args, 1, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ForceJumpEffect>(oTarget, nAdvanced);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectSleep(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<SleepEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectTemporaryForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nTempForce = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<TemporaryForcePointsEffect>(nTempForce);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectConfused(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<ConfusedEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectFrightened(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<FrightenedEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectChoke(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<ChokeEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectStunned(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<StunnedEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectRegenerate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAmount = getInt(args, 0);
    auto fIntervalSeconds = getFloat(args, 1);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<RegenerateEffect>(nAmount, fIntervalSeconds);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectMovementSpeedIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNewSpeedPercent = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<MovementSpeedIncreaseEffect>(nNewSpeedPercent);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectAreaOfEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAreaEffectId = getInt(args, 0);
    auto sOnEnterScript = getStringOrElse(args, 1, "");
    auto sHeartbeatScript = getStringOrElse(args, 2, "");
    auto sOnExitScript = getStringOrElse(args, 3, "");

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<AreaOfEffectEffect>(nAreaEffectId, sOnEnterScript, sHeartbeatScript, sOnExitScript);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectVisualEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nVisualEffectId = getInt(args, 0);
    auto nMissEffect = getIntOrElse(args, 1, 0);

    // Transform
    bool missEffect = static_cast<bool>(nMissEffect);

    // Execute
    auto effect = ctx.game.newEffect<VisualEffect>(nVisualEffectId, missEffect);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectLinkEffects(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eChildEffect = getEffect(args, 0);
    auto eParentEffect = getEffect(args, 1);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<LinkEffectsEffect>(std::move(eChildEffect), std::move(eParentEffect));
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectBeam(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nBeamVisualEffect = getInt(args, 0);
    auto oEffector = getObject(args, 1, ctx);
    auto nBodyPart = getInt(args, 2);
    auto bMissEffect = getIntOrElse(args, 3, 0);

    // Transform
    auto bodyPart = static_cast<BodyNode>(nBodyPart);
    bool missEffect = static_cast<bool>(bMissEffect);

    // Execute
    auto effect = ctx.game.newEffect<BeamEffect>(nBeamVisualEffect, std::move(oEffector), bodyPart, missEffect);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForceResistanceIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nValue = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ForceResistanceIncreaseEffect>(nValue);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectBodyFuel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<BodyFuelEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectPoison(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPoisonType = getInt(args, 0);

    // Transform
    auto poisonType = static_cast<Poison>(nPoisonType);

    // Execute
    auto effect = ctx.game.newEffect<PoisonEffect>(poisonType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectAssuredDeflection(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nReturn = getIntOrElse(args, 0, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<AssuredDeflectionEffect>(nReturn);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForcePushTargeted(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lCentre = getLocationArgument(args, 0);
    auto nIgnoreTestDirectLine = getIntOrElse(args, 1, 0);

    // Transform
    auto ignoreTestDirectLine = static_cast<bool>(nIgnoreTestDirectLine);

    // Execute
    auto effect = ctx.game.newEffect<ForcePushTargetedEffect>(lCentre, ignoreTestDirectLine);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectHaste(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<HasteEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectImmunity(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nImmunityType = getInt(args, 0);

    // Transform
    auto immunityType = static_cast<ImmunityType>(nImmunityType);

    // Execute
    auto effect = ctx.game.newEffect<ImmunityEffect>(immunityType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDamageImmunityIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamageType = getInt(args, 0);
    auto nPercentImmunity = getInt(args, 1);

    // Transform
    auto damageType = static_cast<DamageType>(nDamageType);

    // Execute
    auto effect = ctx.game.newEffect<DamageImmunityIncreaseEffect>(damageType, nPercentImmunity);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectTemporaryHitpoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nHitPoints = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<TemporaryHitPointsEffect>(nHitPoints);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectSkillIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);
    auto nValue = getInt(args, 1);

    // Transform
    auto skill = static_cast<SkillType>(nSkill);

    // Execute
    auto effect = ctx.game.newEffect<SkillIncreaseEffect>(skill, nValue);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDamageForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamage = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<DamageForcePointsEffect>(nDamage);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectHealForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nHeal = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<HealForcePointsEffect>(nHeal);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectHitPointChangeWhenDying(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fHitPointChangePerRound = getFloat(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<HitPointChangeWhenDyingEffect>(fHitPointChangePerRound);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDroidStun(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<DroidStunEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForcePushed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<ForcePushedEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForceResisted(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSource = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ForceResistedEffect>(*oSource);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForceFizzle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<ForceFizzleEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectAbilityDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAbility = getInt(args, 0);
    auto nModifyBy = getInt(args, 1);

    // Transform
    auto ability = static_cast<Ability>(nAbility);

    // Execute
    auto effect = ctx.game.newEffect<AbilityDecreaseEffect>(ability, nModifyBy);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectAttackDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPenalty = getInt(args, 0);
    auto nModifierType = getIntOrElse(args, 1, 0);

    // Transform
    auto modifierType = static_cast<AttackBonus>(nModifierType);

    // Execute
    auto effect = ctx.game.newEffect<AttackDecreaseEffect>(nPenalty, modifierType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDamageDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPenalty = getInt(args, 0);
    auto nDamageType = getIntOrElse(args, 1, 8);

    // Transform
    auto damageType = static_cast<DamageType>(nDamageType);

    // Execute
    auto effect = ctx.game.newEffect<DamageDecreaseEffect>(nPenalty, damageType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDamageImmunityDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamageType = getInt(args, 0);
    auto nPercentImmunity = getInt(args, 1);

    // Transform
    auto damageType = static_cast<DamageType>(nDamageType);

    // Execute
    auto effect = ctx.game.newEffect<DamageImmunityDecreaseEffect>(damageType, nPercentImmunity);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectACDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nValue = getInt(args, 0);
    auto nModifyType = getIntOrElse(args, 1, 0);
    auto nDamageType = getIntOrElse(args, 2, 8199);

    // Transform
    auto modifyType = static_cast<ACBonus>(nModifyType);

    // Execute
    auto effect = ctx.game.newEffect<ACDecreaseEffect>(nValue, modifyType, nDamageType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectMovementSpeedDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPercentChange = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<MovementSpeedDecreaseEffect>(nPercentChange);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectSavingThrowDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSave = getInt(args, 0);
    auto nValue = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);

    // Transform
    auto saveType = static_cast<SavingThrowType>(nSaveType);

    // Execute
    auto effect = ctx.game.newEffect<SavingThrowDecreaseEffect>(nSave, nValue, saveType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectSkillDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);
    auto nValue = getInt(args, 1);

    // Transform
    auto skill = static_cast<SkillType>(nSkill);

    // Execute
    auto effect = ctx.game.newEffect<SkillDecreaseEffect>(skill, nValue);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForceResistanceDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nValue = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ForceResistanceDecreaseEffect>(nValue);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectInvisibility(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInvisibilityType = getInt(args, 0);

    // Transform
    auto invisibilityType = static_cast<InvisibilityType>(nInvisibilityType);

    // Execute
    auto effect = ctx.game.newEffect<InvisibilityEffect>(invisibilityType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectConcealment(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPercentage = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ConcealmentEffect>(nPercentage);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForceShield(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nShield = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ForceShieldEffect>(nShield);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDispelMagicAll(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCasterLevel = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<DispelMagicAllEffect>(nCasterLevel);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDisguise(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDisguiseAppearance = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<DisguiseEffect>(nDisguiseAppearance);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectTrueSeeing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<TrueSeeingEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectSeeInvisible(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<SeeInvisibleEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectTimeStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<TimeStopEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectBlasterDeflectionIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nChange = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<BlasterDeflectionIncreaseEffect>(nChange);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectBlasterDeflectionDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nChange = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<BlasterDeflectionDecreaseEffect>(nChange);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectHorrified(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<HorrifiedEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectSpellLevelAbsorption(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nMaxSpellLevelAbsorbed = getInt(args, 0);
    auto nTotalSpellLevelsAbsorbed = getIntOrElse(args, 1, 0);
    auto nSpellSchool = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<SpellLevelAbsorptionEffect>(nMaxSpellLevelAbsorbed, nTotalSpellLevelsAbsorbed, nSpellSchool);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDispelMagicBest(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCasterLevel = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<DispelMagicBestEffect>(nCasterLevel);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectMissChance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPercentage = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<MissChanceEffect>(nPercentage);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectModifyAttacks(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAttacks = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ModifyAttacksEffect>(nAttacks);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDamageShield(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamageAmount = getInt(args, 0);
    auto nRandomAmount = getInt(args, 1);
    auto nDamageType = getInt(args, 2);

    // Transform
    auto damageType = static_cast<DamageType>(nDamageType);

    // Execute
    auto effect = ctx.game.newEffect<DamageShieldEffect>(nDamageAmount, nRandomAmount, damageType);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForceDrain(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamage = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ForceDrainEffect>(nDamage);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectPsychicStatic(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<PsychicStaticEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectLightsaberThrow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget1 = getObject(args, 0, ctx);
    auto oTarget2 = getObjectOrNull(args, 1, ctx);
    auto oTarget3 = getObjectOrNull(args, 2, ctx);
    auto nAdvancedDamage = getIntOrElse(args, 3, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<LightsaberThrowEffect>(
        std::move(oTarget1),
        std::move(oTarget2),
        std::move(oTarget3),
        nAdvancedDamage);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectWhirlWind(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<WhirlWindEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectCutSceneHorrified(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<CutsceneHorrifiedEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectCutSceneParalyze(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<CutsceneParalyzeEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectCutSceneStunned(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<CutsceneStunnedEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForceBody(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nLevel = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<ForceBodyEffect>(nLevel);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectFury(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<FuryEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectBlind(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<BlindEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectFPRegenModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPercent = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<FPRegenModifierEffect>(nPercent);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectVPRegenModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPercent = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<VPRegenModifierEffect>(nPercent);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectCrush(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<CrushEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDroidConfused(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<DroidConfusedEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectForceSight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<ForceSightEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectMindTrick(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<MindTrickEffect>();
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectFactionModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNewFaction = getInt(args, 0);

    // Transform

    // Execute
    auto effect = ctx.game.newEffect<FactionModifierEffect>(nNewFaction);
    return Variable::ofEffect(std::move(effect));
}

static Variable EffectDroidScramble(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto effect = ctx.game.newEffect<DroidScrambleEffect>();
    return Variable::ofEffect(std::move(effect));
}

void Routines::registerEffectKotorRoutines() {
    insert(51, "EffectAssuredHit", R_EFFECT, {}, &EffectAssuredHit);
    insert(78, "EffectHeal", R_EFFECT, {R_INT}, &EffectHeal);
    insert(79, "EffectDamage", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectDamage);
    insert(80, "EffectAbilityIncrease", R_EFFECT, {R_INT, R_INT}, &EffectAbilityIncrease);
    insert(81, "EffectDamageResistance", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectDamageResistance);
    insert(82, "EffectResurrection", R_EFFECT, {}, &EffectResurrection);
    insert(115, "EffectACIncrease", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectACIncrease);
    insert(117, "EffectSavingThrowIncrease", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectSavingThrowIncrease);
    insert(118, "EffectAttackIncrease", R_EFFECT, {R_INT, R_INT}, &EffectAttackIncrease);
    insert(119, "EffectDamageReduction", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectDamageReduction);
    insert(120, "EffectDamageIncrease", R_EFFECT, {R_INT, R_INT}, &EffectDamageIncrease);
    insert(130, "EffectEntangle", R_EFFECT, {}, &EffectEntangle);
    insert(133, "EffectDeath", R_EFFECT, {R_INT, R_INT}, &EffectDeath);
    insert(134, "EffectKnockdown", R_EFFECT, {}, &EffectKnockdown);
    insert(148, "EffectParalyze", R_EFFECT, {}, &EffectParalyze);
    insert(149, "EffectSpellImmunity", R_EFFECT, {R_INT}, &EffectSpellImmunity);
    insert(153, "EffectForceJump", R_EFFECT, {R_OBJECT, R_INT}, &EffectForceJump);
    insert(154, "EffectSleep", R_EFFECT, {}, &EffectSleep);
    insert(156, "EffectTemporaryForcePoints", R_EFFECT, {R_INT}, &EffectTemporaryForcePoints);
    insert(157, "EffectConfused", R_EFFECT, {}, &EffectConfused);
    insert(158, "EffectFrightened", R_EFFECT, {}, &EffectFrightened);
    insert(159, "EffectChoke", R_EFFECT, {}, &EffectChoke);
    insert(161, "EffectStunned", R_EFFECT, {}, &EffectStunned);
    insert(164, "EffectRegenerate", R_EFFECT, {R_INT, R_FLOAT}, &EffectRegenerate);
    insert(165, "EffectMovementSpeedIncrease", R_EFFECT, {R_INT}, &EffectMovementSpeedIncrease);
    insert(171, "EffectAreaOfEffect", R_EFFECT, {R_INT, R_STRING, R_STRING, R_STRING}, &EffectAreaOfEffect);
    insert(180, "EffectVisualEffect", R_EFFECT, {R_INT, R_INT}, &EffectVisualEffect);
    insert(199, "EffectLinkEffects", R_EFFECT, {R_EFFECT, R_EFFECT}, &EffectLinkEffects);
    insert(207, "EffectBeam", R_EFFECT, {R_INT, R_OBJECT, R_INT, R_INT}, &EffectBeam);
    insert(212, "EffectForceResistanceIncrease", R_EFFECT, {R_INT}, &EffectForceResistanceIncrease);
    insert(224, "EffectBodyFuel", R_EFFECT, {}, &EffectBodyFuel);
    insert(250, "EffectPoison", R_EFFECT, {R_INT}, &EffectPoison);
    insert(252, "EffectAssuredDeflection", R_EFFECT, {R_INT}, &EffectAssuredDeflection);
    insert(269, "EffectForcePushTargeted", R_EFFECT, {R_LOCATION, R_INT}, &EffectForcePushTargeted);
    insert(270, "EffectHaste", R_EFFECT, {}, &EffectHaste);
    insert(273, "EffectImmunity", R_EFFECT, {R_INT}, &EffectImmunity);
    insert(275, "EffectDamageImmunityIncrease", R_EFFECT, {R_INT, R_INT}, &EffectDamageImmunityIncrease);
    insert(314, "EffectTemporaryHitpoints", R_EFFECT, {R_INT}, &EffectTemporaryHitpoints);
    insert(351, "EffectSkillIncrease", R_EFFECT, {R_INT, R_INT}, &EffectSkillIncrease);
    insert(372, "EffectDamageForcePoints", R_EFFECT, {R_INT}, &EffectDamageForcePoints);
    insert(373, "EffectHealForcePoints", R_EFFECT, {R_INT}, &EffectHealForcePoints);
    insert(387, "EffectHitPointChangeWhenDying", R_EFFECT, {R_FLOAT}, &EffectHitPointChangeWhenDying);
    insert(391, "EffectDroidStun", R_EFFECT, {}, &EffectDroidStun);
    insert(392, "EffectForcePushed", R_EFFECT, {}, &EffectForcePushed);
    insert(402, "EffectForceResisted", R_EFFECT, {R_OBJECT}, &EffectForceResisted);
    insert(420, "EffectForceFizzle", R_EFFECT, {}, &EffectForceFizzle);
    insert(446, "EffectAbilityDecrease", R_EFFECT, {R_INT, R_INT}, &EffectAbilityDecrease);
    insert(447, "EffectAttackDecrease", R_EFFECT, {R_INT, R_INT}, &EffectAttackDecrease);
    insert(448, "EffectDamageDecrease", R_EFFECT, {R_INT, R_INT}, &EffectDamageDecrease);
    insert(449, "EffectDamageImmunityDecrease", R_EFFECT, {R_INT, R_INT}, &EffectDamageImmunityDecrease);
    insert(450, "EffectACDecrease", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectACDecrease);
    insert(451, "EffectMovementSpeedDecrease", R_EFFECT, {R_INT}, &EffectMovementSpeedDecrease);
    insert(452, "EffectSavingThrowDecrease", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectSavingThrowDecrease);
    insert(453, "EffectSkillDecrease", R_EFFECT, {R_INT, R_INT}, &EffectSkillDecrease);
    insert(454, "EffectForceResistanceDecrease", R_EFFECT, {R_INT}, &EffectForceResistanceDecrease);
    insert(457, "EffectInvisibility", R_EFFECT, {R_INT}, &EffectInvisibility);
    insert(458, "EffectConcealment", R_EFFECT, {R_INT}, &EffectConcealment);
    insert(459, "EffectForceShield", R_EFFECT, {R_INT}, &EffectForceShield);
    insert(460, "EffectDispelMagicAll", R_EFFECT, {R_INT}, &EffectDispelMagicAll);
    insert(463, "EffectDisguise", R_EFFECT, {R_INT}, &EffectDisguise);
    insert(465, "EffectTrueSeeing", R_EFFECT, {}, &EffectTrueSeeing);
    insert(466, "EffectSeeInvisible", R_EFFECT, {}, &EffectSeeInvisible);
    insert(467, "EffectTimeStop", R_EFFECT, {}, &EffectTimeStop);
    insert(469, "EffectBlasterDeflectionIncrease", R_EFFECT, {R_INT}, &EffectBlasterDeflectionIncrease);
    insert(470, "EffectBlasterDeflectionDecrease", R_EFFECT, {R_INT}, &EffectBlasterDeflectionDecrease);
    insert(471, "EffectHorrified", R_EFFECT, {}, &EffectHorrified);
    insert(472, "EffectSpellLevelAbsorption", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectSpellLevelAbsorption);
    insert(473, "EffectDispelMagicBest", R_EFFECT, {R_INT}, &EffectDispelMagicBest);
    insert(477, "EffectMissChance", R_EFFECT, {R_INT}, &EffectMissChance);
    insert(485, "EffectModifyAttacks", R_EFFECT, {R_INT}, &EffectModifyAttacks);
    insert(487, "EffectDamageShield", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectDamageShield);
    insert(675, "EffectForceDrain", R_EFFECT, {R_INT}, &EffectForceDrain);
    insert(676, "EffectPsychicStatic", R_EFFECT, {}, &EffectPsychicStatic);
    insert(702, "EffectLightsaberThrow", R_EFFECT, {R_OBJECT, R_OBJECT, R_OBJECT, R_INT}, &EffectLightsaberThrow);
    insert(703, "EffectWhirlWind", R_EFFECT, {}, &EffectWhirlWind);
    insert(754, "EffectCutSceneHorrified", R_EFFECT, {}, &EffectCutSceneHorrified);
    insert(755, "EffectCutSceneParalyze", R_EFFECT, {}, &EffectCutSceneParalyze);
    insert(756, "EffectCutSceneStunned", R_EFFECT, {}, &EffectCutSceneStunned);
}

void Routines::registerEffectTslRoutines() {
    insert(51, "EffectAssuredHit", R_EFFECT, {}, &EffectAssuredHit);
    insert(78, "EffectHeal", R_EFFECT, {R_INT}, &EffectHeal);
    insert(79, "EffectDamage", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectDamage);
    insert(80, "EffectAbilityIncrease", R_EFFECT, {R_INT, R_INT}, &EffectAbilityIncrease);
    insert(81, "EffectDamageResistance", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectDamageResistance);
    insert(82, "EffectResurrection", R_EFFECT, {R_INT}, &EffectResurrection);
    insert(115, "EffectACIncrease", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectACIncrease);
    insert(117, "EffectSavingThrowIncrease", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectSavingThrowIncrease);
    insert(118, "EffectAttackIncrease", R_EFFECT, {R_INT, R_INT}, &EffectAttackIncrease);
    insert(119, "EffectDamageReduction", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectDamageReduction);
    insert(120, "EffectDamageIncrease", R_EFFECT, {R_INT, R_INT}, &EffectDamageIncrease);
    insert(130, "EffectEntangle", R_EFFECT, {}, &EffectEntangle);
    insert(133, "EffectDeath", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectDeath);
    insert(134, "EffectKnockdown", R_EFFECT, {}, &EffectKnockdown);
    insert(148, "EffectParalyze", R_EFFECT, {}, &EffectParalyze);
    insert(149, "EffectSpellImmunity", R_EFFECT, {R_INT}, &EffectSpellImmunity);
    insert(153, "EffectForceJump", R_EFFECT, {R_OBJECT, R_INT}, &EffectForceJump);
    insert(154, "EffectSleep", R_EFFECT, {}, &EffectSleep);
    insert(156, "EffectTemporaryForcePoints", R_EFFECT, {R_INT}, &EffectTemporaryForcePoints);
    insert(157, "EffectConfused", R_EFFECT, {}, &EffectConfused);
    insert(158, "EffectFrightened", R_EFFECT, {}, &EffectFrightened);
    insert(159, "EffectChoke", R_EFFECT, {}, &EffectChoke);
    insert(161, "EffectStunned", R_EFFECT, {}, &EffectStunned);
    insert(164, "EffectRegenerate", R_EFFECT, {R_INT, R_FLOAT}, &EffectRegenerate);
    insert(165, "EffectMovementSpeedIncrease", R_EFFECT, {R_INT}, &EffectMovementSpeedIncrease);
    insert(171, "EffectAreaOfEffect", R_EFFECT, {R_INT, R_STRING, R_STRING, R_STRING}, &EffectAreaOfEffect);
    insert(180, "EffectVisualEffect", R_EFFECT, {R_INT, R_INT}, &EffectVisualEffect);
    insert(199, "EffectLinkEffects", R_EFFECT, {R_EFFECT, R_EFFECT}, &EffectLinkEffects);
    insert(207, "EffectBeam", R_EFFECT, {R_INT, R_OBJECT, R_INT, R_INT}, &EffectBeam);
    insert(212, "EffectForceResistanceIncrease", R_EFFECT, {R_INT}, &EffectForceResistanceIncrease);
    insert(224, "EffectBodyFuel", R_EFFECT, {}, &EffectBodyFuel);
    insert(250, "EffectPoison", R_EFFECT, {R_INT}, &EffectPoison);
    insert(252, "EffectAssuredDeflection", R_EFFECT, {R_INT}, &EffectAssuredDeflection);
    insert(269, "EffectForcePushTargeted", R_EFFECT, {R_LOCATION, R_INT}, &EffectForcePushTargeted);
    insert(270, "EffectHaste", R_EFFECT, {}, &EffectHaste);
    insert(273, "EffectImmunity", R_EFFECT, {R_INT}, &EffectImmunity);
    insert(275, "EffectDamageImmunityIncrease", R_EFFECT, {R_INT, R_INT}, &EffectDamageImmunityIncrease);
    insert(314, "EffectTemporaryHitpoints", R_EFFECT, {R_INT}, &EffectTemporaryHitpoints);
    insert(351, "EffectSkillIncrease", R_EFFECT, {R_INT, R_INT}, &EffectSkillIncrease);
    insert(372, "EffectDamageForcePoints", R_EFFECT, {R_INT}, &EffectDamageForcePoints);
    insert(373, "EffectHealForcePoints", R_EFFECT, {R_INT}, &EffectHealForcePoints);
    insert(387, "EffectHitPointChangeWhenDying", R_EFFECT, {R_FLOAT}, &EffectHitPointChangeWhenDying);
    insert(391, "EffectDroidStun", R_EFFECT, {}, &EffectDroidStun);
    insert(392, "EffectForcePushed", R_EFFECT, {}, &EffectForcePushed);
    insert(402, "EffectForceResisted", R_EFFECT, {R_OBJECT}, &EffectForceResisted);
    insert(420, "EffectForceFizzle", R_EFFECT, {}, &EffectForceFizzle);
    insert(446, "EffectAbilityDecrease", R_EFFECT, {R_INT, R_INT}, &EffectAbilityDecrease);
    insert(447, "EffectAttackDecrease", R_EFFECT, {R_INT, R_INT}, &EffectAttackDecrease);
    insert(448, "EffectDamageDecrease", R_EFFECT, {R_INT, R_INT}, &EffectDamageDecrease);
    insert(449, "EffectDamageImmunityDecrease", R_EFFECT, {R_INT, R_INT}, &EffectDamageImmunityDecrease);
    insert(450, "EffectACDecrease", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectACDecrease);
    insert(451, "EffectMovementSpeedDecrease", R_EFFECT, {R_INT}, &EffectMovementSpeedDecrease);
    insert(452, "EffectSavingThrowDecrease", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectSavingThrowDecrease);
    insert(453, "EffectSkillDecrease", R_EFFECT, {R_INT, R_INT}, &EffectSkillDecrease);
    insert(454, "EffectForceResistanceDecrease", R_EFFECT, {R_INT}, &EffectForceResistanceDecrease);
    insert(457, "EffectInvisibility", R_EFFECT, {R_INT}, &EffectInvisibility);
    insert(458, "EffectConcealment", R_EFFECT, {R_INT}, &EffectConcealment);
    insert(459, "EffectForceShield", R_EFFECT, {R_INT}, &EffectForceShield);
    insert(460, "EffectDispelMagicAll", R_EFFECT, {R_INT}, &EffectDispelMagicAll);
    insert(463, "EffectDisguise", R_EFFECT, {R_INT}, &EffectDisguise);
    insert(465, "EffectTrueSeeing", R_EFFECT, {}, &EffectTrueSeeing);
    insert(466, "EffectSeeInvisible", R_EFFECT, {}, &EffectSeeInvisible);
    insert(467, "EffectTimeStop", R_EFFECT, {}, &EffectTimeStop);
    insert(469, "EffectBlasterDeflectionIncrease", R_EFFECT, {R_INT}, &EffectBlasterDeflectionIncrease);
    insert(470, "EffectBlasterDeflectionDecrease", R_EFFECT, {R_INT}, &EffectBlasterDeflectionDecrease);
    insert(471, "EffectHorrified", R_EFFECT, {}, &EffectHorrified);
    insert(472, "EffectSpellLevelAbsorption", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectSpellLevelAbsorption);
    insert(473, "EffectDispelMagicBest", R_EFFECT, {R_INT}, &EffectDispelMagicBest);
    insert(477, "EffectMissChance", R_EFFECT, {R_INT}, &EffectMissChance);
    insert(485, "EffectModifyAttacks", R_EFFECT, {R_INT}, &EffectModifyAttacks);
    insert(487, "EffectDamageShield", R_EFFECT, {R_INT, R_INT, R_INT}, &EffectDamageShield);
    insert(675, "EffectForceDrain", R_EFFECT, {R_INT}, &EffectForceDrain);
    insert(676, "EffectPsychicStatic", R_EFFECT, {}, &EffectPsychicStatic);
    insert(702, "EffectLightsaberThrow", R_EFFECT, {R_OBJECT, R_OBJECT, R_OBJECT, R_INT}, &EffectLightsaberThrow);
    insert(703, "EffectWhirlWind", R_EFFECT, {}, &EffectWhirlWind);
    insert(754, "EffectCutSceneHorrified", R_EFFECT, {}, &EffectCutSceneHorrified);
    insert(755, "EffectCutSceneParalyze", R_EFFECT, {}, &EffectCutSceneParalyze);
    insert(756, "EffectCutSceneStunned", R_EFFECT, {}, &EffectCutSceneStunned);
    insert(770, "EffectForceBody", R_EFFECT, {R_INT}, &EffectForceBody);
    insert(777, "EffectFury", R_EFFECT, {}, &EffectFury);
    insert(778, "EffectBlind", R_EFFECT, {}, &EffectBlind);
    insert(779, "EffectFPRegenModifier", R_EFFECT, {R_INT}, &EffectFPRegenModifier);
    insert(780, "EffectVPRegenModifier", R_EFFECT, {R_INT}, &EffectVPRegenModifier);
    insert(781, "EffectCrush", R_EFFECT, {}, &EffectCrush);
    insert(809, "EffectDroidConfused", R_EFFECT, {}, &EffectDroidConfused);
    insert(823, "EffectForceSight", R_EFFECT, {}, &EffectForceSight);
    insert(848, "EffectMindTrick", R_EFFECT, {}, &EffectMindTrick);
    insert(849, "EffectFactionModifier", R_EFFECT, {R_INT}, &EffectFactionModifier);
    insert(852, "EffectDroidScramble", R_EFFECT, {}, &EffectDroidScramble);
}

} // namespace game

} // namespace reone
