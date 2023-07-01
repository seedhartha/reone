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

namespace routine {

static Variable EffectAssuredHit(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectAssuredHit");
}

static Variable EffectHeal(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nDamageToHeal = getInt(args, 0);

    throw RoutineNotImplementedException("EffectHeal");
}

static Variable EffectDamage(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nDamageAmount = getInt(args, 0);
    auto nDamageType = getIntOrElse(args, 1, 8);
    auto nDamagePower = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("EffectDamage");
}

static Variable EffectAbilityIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nAbilityToIncrease = getInt(args, 0);
    auto nModifyBy = getInt(args, 1);

    throw RoutineNotImplementedException("EffectAbilityIncrease");
}

static Variable EffectDamageResistance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nDamageType = getInt(args, 0);
    auto nAmount = getInt(args, 1);
    auto nLimit = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("EffectDamageResistance");
}

static Variable EffectResurrection(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nHPPercent = getIntOrElse(args, 0, 0);

    throw RoutineNotImplementedException("EffectResurrection");
}

static Variable EffectACIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nValue = getInt(args, 0);
    auto nModifyType = getIntOrElse(args, 1, 0);
    auto nDamageType = getIntOrElse(args, 2, 8199);

    throw RoutineNotImplementedException("EffectACIncrease");
}

static Variable EffectSavingThrowIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSave = getInt(args, 0);
    auto nValue = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("EffectSavingThrowIncrease");
}

static Variable EffectAttackIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nBonus = getInt(args, 0);
    auto nModifierType = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("EffectAttackIncrease");
}

static Variable EffectDamageReduction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nAmount = getInt(args, 0);
    auto nDamagePower = getInt(args, 1);
    auto nLimit = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("EffectDamageReduction");
}

static Variable EffectDamageIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nBonus = getInt(args, 0);
    auto nDamageType = getIntOrElse(args, 1, 8);

    throw RoutineNotImplementedException("EffectDamageIncrease");
}

static Variable EffectEntangle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectEntangle");
}

static Variable EffectDeath(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSpectacularDeath = getIntOrElse(args, 0, 0);
    auto nDisplayFeedback = getIntOrElse(args, 1, 1);
    auto nNoFadeAway = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("EffectDeath");
}

static Variable EffectKnockdown(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectKnockdown");
}

static Variable EffectParalyze(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectParalyze");
}

static Variable EffectSpellImmunity(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nImmunityToSpell = getIntOrElse(args, 0, -1);

    throw RoutineNotImplementedException("EffectSpellImmunity");
}

static Variable EffectForceJump(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oTarget = getObject(args, 0, ctx);
    auto nAdvanced = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("EffectForceJump");
}

static Variable EffectSleep(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectSleep");
}

static Variable EffectTemporaryForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nTempForce = getInt(args, 0);

    throw RoutineNotImplementedException("EffectTemporaryForcePoints");
}

static Variable EffectConfused(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectConfused");
}

static Variable EffectFrightened(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectFrightened");
}

static Variable EffectChoke(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectChoke");
}

static Variable EffectStunned(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectStunned");
}

static Variable EffectRegenerate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nAmount = getInt(args, 0);
    auto fIntervalSeconds = getFloat(args, 1);

    throw RoutineNotImplementedException("EffectRegenerate");
}

static Variable EffectMovementSpeedIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nNewSpeedPercent = getInt(args, 0);

    throw RoutineNotImplementedException("EffectMovementSpeedIncrease");
}

static Variable EffectAreaOfEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nAreaEffectId = getInt(args, 0);
    auto sOnEnterScript = getStringOrElse(args, 1, "");
    auto sHeartbeatScript = getStringOrElse(args, 2, "");
    auto sOnExitScript = getStringOrElse(args, 3, "");

    throw RoutineNotImplementedException("EffectAreaOfEffect");
}

static Variable EffectVisualEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nVisualEffectId = getInt(args, 0);
    auto nMissEffect = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("EffectVisualEffect");
}

static Variable EffectLinkEffects(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto eChildEffect = getEffect(args, 0);
    auto eParentEffect = getEffect(args, 1);

    throw RoutineNotImplementedException("EffectLinkEffects");
}

static Variable EffectBeam(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nBeamVisualEffect = getInt(args, 0);
    auto oEffector = getObject(args, 1, ctx);
    auto nBodyPart = getInt(args, 2);
    auto bMissEffect = getIntOrElse(args, 3, 0);

    throw RoutineNotImplementedException("EffectBeam");
}

static Variable EffectForceResistanceIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nValue = getInt(args, 0);

    throw RoutineNotImplementedException("EffectForceResistanceIncrease");
}

static Variable EffectBodyFuel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectBodyFuel");
}

static Variable EffectPoison(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nPoisonType = getInt(args, 0);

    throw RoutineNotImplementedException("EffectPoison");
}

static Variable EffectAssuredDeflection(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nReturn = getIntOrElse(args, 0, 0);

    throw RoutineNotImplementedException("EffectAssuredDeflection");
}

static Variable EffectForcePushTargeted(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto lCentre = getLocationArgument(args, 0);
    auto nIgnoreTestDirectLine = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("EffectForcePushTargeted");
}

static Variable EffectHaste(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectHaste");
}

static Variable EffectImmunity(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nImmunityType = getInt(args, 0);

    throw RoutineNotImplementedException("EffectImmunity");
}

static Variable EffectDamageImmunityIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nDamageType = getInt(args, 0);
    auto nPercentImmunity = getInt(args, 1);

    throw RoutineNotImplementedException("EffectDamageImmunityIncrease");
}

static Variable EffectTemporaryHitpoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nHitPoints = getInt(args, 0);

    throw RoutineNotImplementedException("EffectTemporaryHitpoints");
}

static Variable EffectSkillIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSkill = getInt(args, 0);
    auto nValue = getInt(args, 1);

    throw RoutineNotImplementedException("EffectSkillIncrease");
}

static Variable EffectDamageForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nDamage = getInt(args, 0);

    throw RoutineNotImplementedException("EffectDamageForcePoints");
}

static Variable EffectHealForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nHeal = getInt(args, 0);

    throw RoutineNotImplementedException("EffectHealForcePoints");
}

static Variable EffectHitPointChangeWhenDying(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto fHitPointChangePerRound = getFloat(args, 0);

    throw RoutineNotImplementedException("EffectHitPointChangeWhenDying");
}

static Variable EffectDroidStun(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectDroidStun");
}

static Variable EffectForcePushed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectForcePushed");
}

static Variable EffectForceResisted(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oSource = getObject(args, 0, ctx);

    throw RoutineNotImplementedException("EffectForceResisted");
}

static Variable EffectForceFizzle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectForceFizzle");
}

static Variable EffectAbilityDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nAbility = getInt(args, 0);
    auto nModifyBy = getInt(args, 1);

    throw RoutineNotImplementedException("EffectAbilityDecrease");
}

static Variable EffectAttackDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nPenalty = getInt(args, 0);
    auto nModifierType = getIntOrElse(args, 1, 0);

    throw RoutineNotImplementedException("EffectAttackDecrease");
}

static Variable EffectDamageDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nPenalty = getInt(args, 0);
    auto nDamageType = getIntOrElse(args, 1, 8);

    throw RoutineNotImplementedException("EffectDamageDecrease");
}

static Variable EffectDamageImmunityDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nDamageType = getInt(args, 0);
    auto nPercentImmunity = getInt(args, 1);

    throw RoutineNotImplementedException("EffectDamageImmunityDecrease");
}

static Variable EffectACDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nValue = getInt(args, 0);
    auto nModifyType = getIntOrElse(args, 1, 0);
    auto nDamageType = getIntOrElse(args, 2, 8199);

    throw RoutineNotImplementedException("EffectACDecrease");
}

static Variable EffectMovementSpeedDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nPercentChange = getInt(args, 0);

    throw RoutineNotImplementedException("EffectMovementSpeedDecrease");
}

static Variable EffectSavingThrowDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSave = getInt(args, 0);
    auto nValue = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("EffectSavingThrowDecrease");
}

static Variable EffectSkillDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nSkill = getInt(args, 0);
    auto nValue = getInt(args, 1);

    throw RoutineNotImplementedException("EffectSkillDecrease");
}

static Variable EffectForceResistanceDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nValue = getInt(args, 0);

    throw RoutineNotImplementedException("EffectForceResistanceDecrease");
}

static Variable EffectInvisibility(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nInvisibilityType = getInt(args, 0);

    throw RoutineNotImplementedException("EffectInvisibility");
}

static Variable EffectConcealment(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nPercentage = getInt(args, 0);

    throw RoutineNotImplementedException("EffectConcealment");
}

static Variable EffectForceShield(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nShield = getInt(args, 0);

    throw RoutineNotImplementedException("EffectForceShield");
}

static Variable EffectDispelMagicAll(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nCasterLevel = getInt(args, 0);

    throw RoutineNotImplementedException("EffectDispelMagicAll");
}

static Variable EffectDisguise(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nDisguiseAppearance = getInt(args, 0);

    throw RoutineNotImplementedException("EffectDisguise");
}

static Variable EffectTrueSeeing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectTrueSeeing");
}

static Variable EffectSeeInvisible(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectSeeInvisible");
}

static Variable EffectTimeStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectTimeStop");
}

static Variable EffectBlasterDeflectionIncrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nChange = getInt(args, 0);

    throw RoutineNotImplementedException("EffectBlasterDeflectionIncrease");
}

static Variable EffectBlasterDeflectionDecrease(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nChange = getInt(args, 0);

    throw RoutineNotImplementedException("EffectBlasterDeflectionDecrease");
}

static Variable EffectHorrified(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectHorrified");
}

static Variable EffectSpellLevelAbsorption(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nMaxSpellLevelAbsorbed = getInt(args, 0);
    auto nTotalSpellLevelsAbsorbed = getIntOrElse(args, 1, 0);
    auto nSpellSchool = getIntOrElse(args, 2, 0);

    throw RoutineNotImplementedException("EffectSpellLevelAbsorption");
}

static Variable EffectDispelMagicBest(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nCasterLevel = getInt(args, 0);

    throw RoutineNotImplementedException("EffectDispelMagicBest");
}

static Variable EffectMissChance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nPercentage = getInt(args, 0);

    throw RoutineNotImplementedException("EffectMissChance");
}

static Variable EffectModifyAttacks(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nAttacks = getInt(args, 0);

    throw RoutineNotImplementedException("EffectModifyAttacks");
}

static Variable EffectDamageShield(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nDamageAmount = getInt(args, 0);
    auto nRandomAmount = getInt(args, 1);
    auto nDamageType = getInt(args, 2);

    throw RoutineNotImplementedException("EffectDamageShield");
}

static Variable EffectForceDrain(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nDamage = getInt(args, 0);

    throw RoutineNotImplementedException("EffectForceDrain");
}

static Variable EffectPsychicStatic(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectPsychicStatic");
}

static Variable EffectLightsaberThrow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto oTarget1 = getObject(args, 0, ctx);
    auto oTarget2 = getObjectOrNull(args, 1, ctx);
    auto oTarget3 = getObjectOrNull(args, 2, ctx);
    auto nAdvancedDamage = getIntOrElse(args, 3, 0);

    throw RoutineNotImplementedException("EffectLightsaberThrow");
}

static Variable EffectWhirlWind(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectWhirlWind");
}

static Variable EffectCutSceneHorrified(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectCutSceneHorrified");
}

static Variable EffectCutSceneParalyze(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectCutSceneParalyze");
}

static Variable EffectCutSceneStunned(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectCutSceneStunned");
}

static Variable EffectForceBody(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nLevel = getInt(args, 0);

    throw RoutineNotImplementedException("EffectForceBody");
}

static Variable EffectFury(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectFury");
}

static Variable EffectBlind(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectBlind");
}

static Variable EffectFPRegenModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nPercent = getInt(args, 0);

    throw RoutineNotImplementedException("EffectFPRegenModifier");
}

static Variable EffectVPRegenModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nPercent = getInt(args, 0);

    throw RoutineNotImplementedException("EffectVPRegenModifier");
}

static Variable EffectCrush(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectCrush");
}

static Variable EffectDroidConfused(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectDroidConfused");
}

static Variable EffectForceSight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectForceSight");
}

static Variable EffectMindTrick(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectMindTrick");
}

static Variable EffectFactionModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    auto nNewFaction = getInt(args, 0);

    throw RoutineNotImplementedException("EffectFactionModifier");
}

static Variable EffectDroidScramble(const std::vector<Variable> &args, const RoutineContext &ctx) {
    throw RoutineNotImplementedException("EffectDroidScramble");
}

} // namespace routine

void registerEffectKotorRoutines(Routines &routines) {
    routines.insert(51, "EffectAssuredHit", R_EFFECT, {}, &routine::EffectAssuredHit);
    routines.insert(78, "EffectHeal", R_EFFECT, {R_INT}, &routine::EffectHeal);
    routines.insert(79, "EffectDamage", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectDamage);
    routines.insert(80, "EffectAbilityIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectAbilityIncrease);
    routines.insert(81, "EffectDamageResistance", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectDamageResistance);
    routines.insert(82, "EffectResurrection", R_EFFECT, {}, &routine::EffectResurrection);
    routines.insert(115, "EffectACIncrease", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectACIncrease);
    routines.insert(117, "EffectSavingThrowIncrease", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectSavingThrowIncrease);
    routines.insert(118, "EffectAttackIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectAttackIncrease);
    routines.insert(119, "EffectDamageReduction", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectDamageReduction);
    routines.insert(120, "EffectDamageIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectDamageIncrease);
    routines.insert(130, "EffectEntangle", R_EFFECT, {}, &routine::EffectEntangle);
    routines.insert(133, "EffectDeath", R_EFFECT, {R_INT, R_INT}, &routine::EffectDeath);
    routines.insert(134, "EffectKnockdown", R_EFFECT, {}, &routine::EffectKnockdown);
    routines.insert(148, "EffectParalyze", R_EFFECT, {}, &routine::EffectParalyze);
    routines.insert(149, "EffectSpellImmunity", R_EFFECT, {R_INT}, &routine::EffectSpellImmunity);
    routines.insert(153, "EffectForceJump", R_EFFECT, {R_OBJECT, R_INT}, &routine::EffectForceJump);
    routines.insert(154, "EffectSleep", R_EFFECT, {}, &routine::EffectSleep);
    routines.insert(156, "EffectTemporaryForcePoints", R_EFFECT, {R_INT}, &routine::EffectTemporaryForcePoints);
    routines.insert(157, "EffectConfused", R_EFFECT, {}, &routine::EffectConfused);
    routines.insert(158, "EffectFrightened", R_EFFECT, {}, &routine::EffectFrightened);
    routines.insert(159, "EffectChoke", R_EFFECT, {}, &routine::EffectChoke);
    routines.insert(161, "EffectStunned", R_EFFECT, {}, &routine::EffectStunned);
    routines.insert(164, "EffectRegenerate", R_EFFECT, {R_INT, R_FLOAT}, &routine::EffectRegenerate);
    routines.insert(165, "EffectMovementSpeedIncrease", R_EFFECT, {R_INT}, &routine::EffectMovementSpeedIncrease);
    routines.insert(171, "EffectAreaOfEffect", R_EFFECT, {R_INT, R_STRING, R_STRING, R_STRING}, &routine::EffectAreaOfEffect);
    routines.insert(180, "EffectVisualEffect", R_EFFECT, {R_INT, R_INT}, &routine::EffectVisualEffect);
    routines.insert(199, "EffectLinkEffects", R_EFFECT, {R_EFFECT, R_EFFECT}, &routine::EffectLinkEffects);
    routines.insert(207, "EffectBeam", R_EFFECT, {R_INT, R_OBJECT, R_INT, R_INT}, &routine::EffectBeam);
    routines.insert(212, "EffectForceResistanceIncrease", R_EFFECT, {R_INT}, &routine::EffectForceResistanceIncrease);
    routines.insert(224, "EffectBodyFuel", R_EFFECT, {}, &routine::EffectBodyFuel);
    routines.insert(250, "EffectPoison", R_EFFECT, {R_INT}, &routine::EffectPoison);
    routines.insert(252, "EffectAssuredDeflection", R_EFFECT, {R_INT}, &routine::EffectAssuredDeflection);
    routines.insert(269, "EffectForcePushTargeted", R_EFFECT, {R_LOCATION, R_INT}, &routine::EffectForcePushTargeted);
    routines.insert(270, "EffectHaste", R_EFFECT, {}, &routine::EffectHaste);
    routines.insert(273, "EffectImmunity", R_EFFECT, {R_INT}, &routine::EffectImmunity);
    routines.insert(275, "EffectDamageImmunityIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectDamageImmunityIncrease);
    routines.insert(314, "EffectTemporaryHitpoints", R_EFFECT, {R_INT}, &routine::EffectTemporaryHitpoints);
    routines.insert(351, "EffectSkillIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectSkillIncrease);
    routines.insert(372, "EffectDamageForcePoints", R_EFFECT, {R_INT}, &routine::EffectDamageForcePoints);
    routines.insert(373, "EffectHealForcePoints", R_EFFECT, {R_INT}, &routine::EffectHealForcePoints);
    routines.insert(387, "EffectHitPointChangeWhenDying", R_EFFECT, {R_FLOAT}, &routine::EffectHitPointChangeWhenDying);
    routines.insert(391, "EffectDroidStun", R_EFFECT, {}, &routine::EffectDroidStun);
    routines.insert(392, "EffectForcePushed", R_EFFECT, {}, &routine::EffectForcePushed);
    routines.insert(402, "EffectForceResisted", R_EFFECT, {R_OBJECT}, &routine::EffectForceResisted);
    routines.insert(420, "EffectForceFizzle", R_EFFECT, {}, &routine::EffectForceFizzle);
    routines.insert(446, "EffectAbilityDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectAbilityDecrease);
    routines.insert(447, "EffectAttackDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectAttackDecrease);
    routines.insert(448, "EffectDamageDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectDamageDecrease);
    routines.insert(449, "EffectDamageImmunityDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectDamageImmunityDecrease);
    routines.insert(450, "EffectACDecrease", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectACDecrease);
    routines.insert(451, "EffectMovementSpeedDecrease", R_EFFECT, {R_INT}, &routine::EffectMovementSpeedDecrease);
    routines.insert(452, "EffectSavingThrowDecrease", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectSavingThrowDecrease);
    routines.insert(453, "EffectSkillDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectSkillDecrease);
    routines.insert(454, "EffectForceResistanceDecrease", R_EFFECT, {R_INT}, &routine::EffectForceResistanceDecrease);
    routines.insert(457, "EffectInvisibility", R_EFFECT, {R_INT}, &routine::EffectInvisibility);
    routines.insert(458, "EffectConcealment", R_EFFECT, {R_INT}, &routine::EffectConcealment);
    routines.insert(459, "EffectForceShield", R_EFFECT, {R_INT}, &routine::EffectForceShield);
    routines.insert(460, "EffectDispelMagicAll", R_EFFECT, {R_INT}, &routine::EffectDispelMagicAll);
    routines.insert(463, "EffectDisguise", R_EFFECT, {R_INT}, &routine::EffectDisguise);
    routines.insert(465, "EffectTrueSeeing", R_EFFECT, {}, &routine::EffectTrueSeeing);
    routines.insert(466, "EffectSeeInvisible", R_EFFECT, {}, &routine::EffectSeeInvisible);
    routines.insert(467, "EffectTimeStop", R_EFFECT, {}, &routine::EffectTimeStop);
    routines.insert(469, "EffectBlasterDeflectionIncrease", R_EFFECT, {R_INT}, &routine::EffectBlasterDeflectionIncrease);
    routines.insert(470, "EffectBlasterDeflectionDecrease", R_EFFECT, {R_INT}, &routine::EffectBlasterDeflectionDecrease);
    routines.insert(471, "EffectHorrified", R_EFFECT, {}, &routine::EffectHorrified);
    routines.insert(472, "EffectSpellLevelAbsorption", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectSpellLevelAbsorption);
    routines.insert(473, "EffectDispelMagicBest", R_EFFECT, {R_INT}, &routine::EffectDispelMagicBest);
    routines.insert(477, "EffectMissChance", R_EFFECT, {R_INT}, &routine::EffectMissChance);
    routines.insert(485, "EffectModifyAttacks", R_EFFECT, {R_INT}, &routine::EffectModifyAttacks);
    routines.insert(487, "EffectDamageShield", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectDamageShield);
    routines.insert(675, "EffectForceDrain", R_EFFECT, {R_INT}, &routine::EffectForceDrain);
    routines.insert(676, "EffectPsychicStatic", R_EFFECT, {}, &routine::EffectPsychicStatic);
    routines.insert(702, "EffectLightsaberThrow", R_EFFECT, {R_OBJECT, R_OBJECT, R_OBJECT, R_INT}, &routine::EffectLightsaberThrow);
    routines.insert(703, "EffectWhirlWind", R_EFFECT, {}, &routine::EffectWhirlWind);
    routines.insert(754, "EffectCutSceneHorrified", R_EFFECT, {}, &routine::EffectCutSceneHorrified);
    routines.insert(755, "EffectCutSceneParalyze", R_EFFECT, {}, &routine::EffectCutSceneParalyze);
    routines.insert(756, "EffectCutSceneStunned", R_EFFECT, {}, &routine::EffectCutSceneStunned);
}

void registerEffectTslRoutines(Routines &routines) {
    routines.insert(51, "EffectAssuredHit", R_EFFECT, {}, &routine::EffectAssuredHit);
    routines.insert(78, "EffectHeal", R_EFFECT, {R_INT}, &routine::EffectHeal);
    routines.insert(79, "EffectDamage", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectDamage);
    routines.insert(80, "EffectAbilityIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectAbilityIncrease);
    routines.insert(81, "EffectDamageResistance", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectDamageResistance);
    routines.insert(82, "EffectResurrection", R_EFFECT, {R_INT}, &routine::EffectResurrection);
    routines.insert(115, "EffectACIncrease", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectACIncrease);
    routines.insert(117, "EffectSavingThrowIncrease", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectSavingThrowIncrease);
    routines.insert(118, "EffectAttackIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectAttackIncrease);
    routines.insert(119, "EffectDamageReduction", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectDamageReduction);
    routines.insert(120, "EffectDamageIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectDamageIncrease);
    routines.insert(130, "EffectEntangle", R_EFFECT, {}, &routine::EffectEntangle);
    routines.insert(133, "EffectDeath", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectDeath);
    routines.insert(134, "EffectKnockdown", R_EFFECT, {}, &routine::EffectKnockdown);
    routines.insert(148, "EffectParalyze", R_EFFECT, {}, &routine::EffectParalyze);
    routines.insert(149, "EffectSpellImmunity", R_EFFECT, {R_INT}, &routine::EffectSpellImmunity);
    routines.insert(153, "EffectForceJump", R_EFFECT, {R_OBJECT, R_INT}, &routine::EffectForceJump);
    routines.insert(154, "EffectSleep", R_EFFECT, {}, &routine::EffectSleep);
    routines.insert(156, "EffectTemporaryForcePoints", R_EFFECT, {R_INT}, &routine::EffectTemporaryForcePoints);
    routines.insert(157, "EffectConfused", R_EFFECT, {}, &routine::EffectConfused);
    routines.insert(158, "EffectFrightened", R_EFFECT, {}, &routine::EffectFrightened);
    routines.insert(159, "EffectChoke", R_EFFECT, {}, &routine::EffectChoke);
    routines.insert(161, "EffectStunned", R_EFFECT, {}, &routine::EffectStunned);
    routines.insert(164, "EffectRegenerate", R_EFFECT, {R_INT, R_FLOAT}, &routine::EffectRegenerate);
    routines.insert(165, "EffectMovementSpeedIncrease", R_EFFECT, {R_INT}, &routine::EffectMovementSpeedIncrease);
    routines.insert(171, "EffectAreaOfEffect", R_EFFECT, {R_INT, R_STRING, R_STRING, R_STRING}, &routine::EffectAreaOfEffect);
    routines.insert(180, "EffectVisualEffect", R_EFFECT, {R_INT, R_INT}, &routine::EffectVisualEffect);
    routines.insert(199, "EffectLinkEffects", R_EFFECT, {R_EFFECT, R_EFFECT}, &routine::EffectLinkEffects);
    routines.insert(207, "EffectBeam", R_EFFECT, {R_INT, R_OBJECT, R_INT, R_INT}, &routine::EffectBeam);
    routines.insert(212, "EffectForceResistanceIncrease", R_EFFECT, {R_INT}, &routine::EffectForceResistanceIncrease);
    routines.insert(224, "EffectBodyFuel", R_EFFECT, {}, &routine::EffectBodyFuel);
    routines.insert(250, "EffectPoison", R_EFFECT, {R_INT}, &routine::EffectPoison);
    routines.insert(252, "EffectAssuredDeflection", R_EFFECT, {R_INT}, &routine::EffectAssuredDeflection);
    routines.insert(269, "EffectForcePushTargeted", R_EFFECT, {R_LOCATION, R_INT}, &routine::EffectForcePushTargeted);
    routines.insert(270, "EffectHaste", R_EFFECT, {}, &routine::EffectHaste);
    routines.insert(273, "EffectImmunity", R_EFFECT, {R_INT}, &routine::EffectImmunity);
    routines.insert(275, "EffectDamageImmunityIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectDamageImmunityIncrease);
    routines.insert(314, "EffectTemporaryHitpoints", R_EFFECT, {R_INT}, &routine::EffectTemporaryHitpoints);
    routines.insert(351, "EffectSkillIncrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectSkillIncrease);
    routines.insert(372, "EffectDamageForcePoints", R_EFFECT, {R_INT}, &routine::EffectDamageForcePoints);
    routines.insert(373, "EffectHealForcePoints", R_EFFECT, {R_INT}, &routine::EffectHealForcePoints);
    routines.insert(387, "EffectHitPointChangeWhenDying", R_EFFECT, {R_FLOAT}, &routine::EffectHitPointChangeWhenDying);
    routines.insert(391, "EffectDroidStun", R_EFFECT, {}, &routine::EffectDroidStun);
    routines.insert(392, "EffectForcePushed", R_EFFECT, {}, &routine::EffectForcePushed);
    routines.insert(402, "EffectForceResisted", R_EFFECT, {R_OBJECT}, &routine::EffectForceResisted);
    routines.insert(420, "EffectForceFizzle", R_EFFECT, {}, &routine::EffectForceFizzle);
    routines.insert(446, "EffectAbilityDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectAbilityDecrease);
    routines.insert(447, "EffectAttackDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectAttackDecrease);
    routines.insert(448, "EffectDamageDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectDamageDecrease);
    routines.insert(449, "EffectDamageImmunityDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectDamageImmunityDecrease);
    routines.insert(450, "EffectACDecrease", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectACDecrease);
    routines.insert(451, "EffectMovementSpeedDecrease", R_EFFECT, {R_INT}, &routine::EffectMovementSpeedDecrease);
    routines.insert(452, "EffectSavingThrowDecrease", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectSavingThrowDecrease);
    routines.insert(453, "EffectSkillDecrease", R_EFFECT, {R_INT, R_INT}, &routine::EffectSkillDecrease);
    routines.insert(454, "EffectForceResistanceDecrease", R_EFFECT, {R_INT}, &routine::EffectForceResistanceDecrease);
    routines.insert(457, "EffectInvisibility", R_EFFECT, {R_INT}, &routine::EffectInvisibility);
    routines.insert(458, "EffectConcealment", R_EFFECT, {R_INT}, &routine::EffectConcealment);
    routines.insert(459, "EffectForceShield", R_EFFECT, {R_INT}, &routine::EffectForceShield);
    routines.insert(460, "EffectDispelMagicAll", R_EFFECT, {R_INT}, &routine::EffectDispelMagicAll);
    routines.insert(463, "EffectDisguise", R_EFFECT, {R_INT}, &routine::EffectDisguise);
    routines.insert(465, "EffectTrueSeeing", R_EFFECT, {}, &routine::EffectTrueSeeing);
    routines.insert(466, "EffectSeeInvisible", R_EFFECT, {}, &routine::EffectSeeInvisible);
    routines.insert(467, "EffectTimeStop", R_EFFECT, {}, &routine::EffectTimeStop);
    routines.insert(469, "EffectBlasterDeflectionIncrease", R_EFFECT, {R_INT}, &routine::EffectBlasterDeflectionIncrease);
    routines.insert(470, "EffectBlasterDeflectionDecrease", R_EFFECT, {R_INT}, &routine::EffectBlasterDeflectionDecrease);
    routines.insert(471, "EffectHorrified", R_EFFECT, {}, &routine::EffectHorrified);
    routines.insert(472, "EffectSpellLevelAbsorption", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectSpellLevelAbsorption);
    routines.insert(473, "EffectDispelMagicBest", R_EFFECT, {R_INT}, &routine::EffectDispelMagicBest);
    routines.insert(477, "EffectMissChance", R_EFFECT, {R_INT}, &routine::EffectMissChance);
    routines.insert(485, "EffectModifyAttacks", R_EFFECT, {R_INT}, &routine::EffectModifyAttacks);
    routines.insert(487, "EffectDamageShield", R_EFFECT, {R_INT, R_INT, R_INT}, &routine::EffectDamageShield);
    routines.insert(675, "EffectForceDrain", R_EFFECT, {R_INT}, &routine::EffectForceDrain);
    routines.insert(676, "EffectPsychicStatic", R_EFFECT, {}, &routine::EffectPsychicStatic);
    routines.insert(702, "EffectLightsaberThrow", R_EFFECT, {R_OBJECT, R_OBJECT, R_OBJECT, R_INT}, &routine::EffectLightsaberThrow);
    routines.insert(703, "EffectWhirlWind", R_EFFECT, {}, &routine::EffectWhirlWind);
    routines.insert(754, "EffectCutSceneHorrified", R_EFFECT, {}, &routine::EffectCutSceneHorrified);
    routines.insert(755, "EffectCutSceneParalyze", R_EFFECT, {}, &routine::EffectCutSceneParalyze);
    routines.insert(756, "EffectCutSceneStunned", R_EFFECT, {}, &routine::EffectCutSceneStunned);
    routines.insert(770, "EffectForceBody", R_EFFECT, {R_INT}, &routine::EffectForceBody);
    routines.insert(777, "EffectFury", R_EFFECT, {}, &routine::EffectFury);
    routines.insert(778, "EffectBlind", R_EFFECT, {}, &routine::EffectBlind);
    routines.insert(779, "EffectFPRegenModifier", R_EFFECT, {R_INT}, &routine::EffectFPRegenModifier);
    routines.insert(780, "EffectVPRegenModifier", R_EFFECT, {R_INT}, &routine::EffectVPRegenModifier);
    routines.insert(781, "EffectCrush", R_EFFECT, {}, &routine::EffectCrush);
    routines.insert(809, "EffectDroidConfused", R_EFFECT, {}, &routine::EffectDroidConfused);
    routines.insert(823, "EffectForceSight", R_EFFECT, {}, &routine::EffectForceSight);
    routines.insert(848, "EffectMindTrick", R_EFFECT, {}, &routine::EffectMindTrick);
    routines.insert(849, "EffectFactionModifier", R_EFFECT, {R_INT}, &routine::EffectFactionModifier);
    routines.insert(852, "EffectDroidScramble", R_EFFECT, {}, &routine::EffectDroidScramble);
}

} // namespace game

} // namespace reone

