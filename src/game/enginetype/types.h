/*
 * Copyright (c) 2020 The reone project contributors
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

namespace reone {

namespace game {

enum class EffectType {
    Invalid = 0,
    DamageResistance = 1,
    AbilityBonus = 2,
    Regenerate = 3,
    SavingThrowBonus = 4,
    ModifyAC = 5,
    AttackBonus = 6,
    DamageReduction = 7,
    DamageBonus = 8,
    TemporaryHitpoints = 9,
    DamageImmunity = 10,
    Entangle = 11,
    Invulnerable = 12,
    Deaf = 13,
    Resurrection = 14,
    Immunity = 15,
    Blind = 16,
    EnemyAttackBonus = 17,
    ArcaneSpellFailure = 18,
    MovementSpeed = 19,
    AreaOfEffect = 20,
    Beam = 21,
    ForceResistance = 22,
    Charmed = 23,
    Confused = 24,
    Frightened = 25,
    Dominated = 26,
    Paralyze = 27,
    Dazed = 28,
    Stunned = 29,
    Sleep = 30,
    Poison = 31,
    Disease = 32,
    Curse = 33,
    Silence = 34,
    Turned = 35,
    Haste = 36,
    Slow = 37,
    AbilityIncrease = 38,
    AbilityDecrease = 39,
    AttackIncrease = 40,
    AttackDecrease = 41,
    DamageIncrease = 42,
    DamageDecrease = 43,
    DamageImmunityIncrease = 44,
    DamageImmunityDecrease = 45,
    ACIncrease = 46,
    ACDecrease = 47,
    MovementSpeedIncrease = 48,
    MovementSpeedDecrease = 49,
    SavingThrowIncrease = 50,
    SavingThrowDecrease = 51,
    ForceResistanceIncrease = 52,
    ForceResistanceDecrease = 53,
    SkillIncrease = 54,
    SkillDecrease = 55,
    Invisibility = 56,
    ImprovedInvisibility = 57,
    Darkness = 58,
    DispelMagicAll = 59,
    ElementalShield = 60,
    NegativeLevel = 61,
    Disguise = 62,
    Sanctuary = 63,
    TrueSeeing = 64,
    SeeInvisible = 65,
    TimeStop = 66,
    Blindness = 67,
    SpellLevelAbsorption = 68,
    DispelMagicBest = 69,
    Ultravision = 70,
    MissChance = 71,
    Concealment = 72,
    SpellImmunity = 73,
    AssuredHit = 74,
    Visual = 75,
    LightsaberThrow = 76,
    ForceJump = 77,
    AssuredDeflection = 78,
    DroidConfused = 79,
    MindTrick = 80,
    DroidScramble = 81,

    Damage = 0x100,
    Heal = 0x101,
    Death = 0x102,
    Knockdown = 0x103,
    TemporaryForcePoints = 0x104,
    Choke = 0x105,
    LinkEffects = 0x106,
    BodyFuel = 0x107,
    ForcePushTargeted = 0x108,
    DamageForcePoints = 0x109,
    HealForcePoints = 0x10a,
    HitPointsChangeWhenDying = 0x10b,
    DroidStun = 0x10c,
    ForcePushed = 0x10d,
    ForceResisted = 0x10e,
    ForceFizzle = 0x10f,
    ForceShield = 0x110,
    BlasterDeflectionIncrease = 0x111,
    BlasterDeflectionDecrease = 0x112,
    Horrified = 0x113,
    ModifyAttacks = 0x114,
    DamageShield = 0x115,
    ForceDrain = 0x116,
    PsychicStatic = 0x117,
    WhirlWind = 0x118,
    CutSceneHorrified = 0x119,
    CutSceneParalyze = 0x11a,
    CutSceneStunned = 0x11b,
    ForceBody = 0x11c,
    Fury = 0x11d,
    FPRegenModifier = 0x11e,
    VPRegenModifier = 0x11f,
    Crush = 0x120,
    ForceSight = 0x121,
    FactionModifier = 0x122
};

} // namespace game

} // namespace reone
