/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/game/script/routine/declarations.h"

#include "reone/script/exception/notimpl.h"

#include "reone/game/effect.h"
#include "reone/game/game.h"
#include "reone/game/services.h"
#include "reone/game/types.h"

#include "reone/game/script/routine/argutil.h"
#include "reone/game/script/routine/context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

static constexpr int kAcVsDamageTypeAll = 8199;

Variable effectAssuredHit(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectHeal(const vector<Variable> &args, const RoutineContext &ctx) {
    int damageToHeal = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDamage(const vector<Variable> &args, const RoutineContext &ctx) {
    int damageAmount = getInt(args, 0);
    auto damageType = getIntAsEnumOrElse(args, 1, DamageType::Universal);
    auto damagePower = getIntAsEnumOrElse(args, 2, DamagePower::Normal);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectAbilityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto abilityToIncrease = getIntAsEnum<Ability>(args, 0);
    int modifyBy = getInt(args, 1);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDamageResistance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto damageType = getIntAsEnum<DamageType>(args, 0);
    int amount = getInt(args, 1);
    int limit = getIntOrElse(args, 2, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectResurrection(const vector<Variable> &args, const RoutineContext &ctx) {
    int hpPercent = getIntOrElse(args, 0, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectACIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);
    auto modifyType = getIntAsEnumOrElse(args, 1, ACBonus::Dodge);
    int damageType = getIntOrElse(args, 2, kAcVsDamageTypeAll);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectSavingThrowIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int save = getInt(args, 0);
    int value = getInt(args, 1);
    auto saveType = getIntAsEnumOrElse(args, 2, SavingThrowType::All);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectAttackIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int bonus = getInt(args, 0);
    auto modifierType = getIntAsEnumOrElse(args, 1, AttackBonus::Misc);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDamageIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectEntangle(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDeath(const vector<Variable> &args, const RoutineContext &ctx) {
    bool spectacularDeath = getIntAsBoolOrElse(args, 0, false);
    bool displayFeedback = getIntAsBoolOrElse(args, 1, true);
    bool noFadeAway = getIntAsBoolOrElse(args, 2, false);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectSleep(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectTemporaryForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    int tempForce = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectChoke(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectRegenerate(const vector<Variable> &args, const RoutineContext &ctx) {
    int amount = getInt(args, 0);
    float intervalSeconds = getFloat(args, 1);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectMovementSpeedIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int newSpeedPercent = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectVisualEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    int visualEffectId = getInt(args, 0);
    bool missEffect = getIntAsBoolOrElse(args, 1, false);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectLinkEffects(const vector<Variable> &args, const RoutineContext &ctx) {
    auto childEffect = getEffect(args, 0);
    auto parentEffect = getEffect(args, 1);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectBeam(const vector<Variable> &args, const RoutineContext &ctx) {
    int beamVisualEffect = getInt(args, 0);
    auto effector = getObject(args, 1, ctx);
    auto bodyPart = getIntAsEnum<BodyNode>(args, 2);
    bool missEffect = getIntAsBoolOrElse(args, 3, false);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectForceResistanceIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectBodyFuel(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectPoison(const vector<Variable> &args, const RoutineContext &ctx) {
    auto poisonType = getIntAsEnum<Poison>(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectForcePushTargeted(const vector<Variable> &args, const RoutineContext &ctx) {
    auto centre = getLocationArgument(args, 0);
    bool ignoreTestDirectLine = getIntAsBoolOrElse(args, 1, false);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectImmunity(const vector<Variable> &args, const RoutineContext &ctx) {
    auto immunityType = getIntAsEnum<ImmunityType>(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDamageImmunityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto damageType = getIntAsEnum<DamageType>(args, 0);
    int percentImmunity = getInt(args, 1);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectTemporaryHitpoints(const vector<Variable> &args, const RoutineContext &ctx) {
    int hitPoints = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectSkillIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto skill = getIntAsEnum<SkillType>(args, 0);
    int value = getInt(args, 1);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDamageForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    int damage = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectHealForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    int heal = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDroidStun(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectForcePushed(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectForceResisted(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectForceFizzle(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectAbilityDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto ability = getIntAsEnum<Ability>(args, 0);
    int modifyBy = getInt(args, 1);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectAttackDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDamageDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int penalty = getInt(args, 0);
    auto modifierType = getIntAsEnumOrElse(args, 1, AttackBonus::Misc);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectACDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int value = getInt(args, 0);
    auto modifyType = getIntAsEnumOrElse(args, 1, ACBonus::Dodge);
    int damageType = getIntOrElse(args, 2, kAcVsDamageTypeAll);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectMovementSpeedDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int percentChange = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectSavingThrowDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int save = getInt(args, 0);
    int value = getInt(args, 1);
    auto saveType = getIntAsEnumOrElse(args, 2, SavingThrowType::All);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectInvisibility(const vector<Variable> &args, const RoutineContext &ctx) {
    auto invisibilityType = getIntAsEnum<InvisibilityType>(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectForceShield(const vector<Variable> &args, const RoutineContext &ctx) {
    int shield = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDisguise(const vector<Variable> &args, const RoutineContext &ctx) {
    int disguiseAppearance = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectTrueSeeing(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectBlasterDeflectionIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    int change = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectModifyAttacks(const vector<Variable> &args, const RoutineContext &ctx) {
    int attacks = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectPsychicStatic(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectLightsaberThrow(const vector<Variable> &args, const RoutineContext &ctx) {
    auto target1 = getObject(args, 0, ctx);
    auto target2 = getObjectOrNull(args, 1, ctx);
    auto target3 = getObjectOrNull(args, 2, ctx);
    int advancedDamage = getInt(args, 3);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectWhirlWind(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectCutSceneHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectCutSceneParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectCutSceneStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

// TSL

Variable effectForceBody(const vector<Variable> &args, const RoutineContext &ctx) {
    int level = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectFury(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectBlind(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectVPRegenModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    int percent = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectCrush(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectForceSight(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectMindTrick(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectFactionModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    int newFaction = getInt(args, 0);

    // TODO: implement

    return Variable::ofEffect(nullptr);
}

Variable effectDroidScramble(const vector<Variable> &args, const RoutineContext &ctx) {
    // TODO: implement

    return Variable::ofEffect(nullptr);
}

// END TSL

} // namespace routine

} // namespace game

} // namespace reone
